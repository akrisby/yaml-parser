// Header-only include resolver for YAML files using yaml-cpp
#pragma once

#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>

namespace yaml_utils
{

    static inline std::filesystem::path join_base(const std::filesystem::path &base, const std::string &rel)
    {
        std::filesystem::path p(rel);
        if (p.is_relative())
            return base / p;
        return p;
    }

    void resolveIncludesRec(YAML::Node &node, const std::filesystem::path &baseDir, std::unordered_set<std::string> &seen);

    inline YAML::Node loadWithIncludes(const std::string &filePath)
    {
        namespace fs = std::filesystem;
        fs::path p = fs::absolute(filePath);
        try
        {
            p = fs::weakly_canonical(p);
        }
        catch (...)
        {
            // weakly_canonical may fail if path doesn't exist; keep absolute
        }

        std::string key = p.string();
        if (key.empty())
            throw std::runtime_error("empty config path");

        std::unordered_set<std::string> seen;
        seen.insert(key);

        // Load ALL top-level documents so we can preserve read order when
        // multiple root-level !include directives are present.
        std::vector<YAML::Node> docs = YAML::LoadAllFromFile(p.string());

        // Resolve includes in each document
        for (auto &doc : docs)
        {
            resolveIncludesRec(doc, p.parent_path(), seen);
        }

        if (docs.empty())
        {
            return YAML::Node();
        }

        if (docs.size() == 1)
        {
            return docs[0];
        }

        // If all docs are maps -> merge maps in read order (later overrides earlier)
        bool allMaps = std::all_of(docs.begin(), docs.end(), [](const YAML::Node &n)
                                   { return n.IsMap(); });
        if (allMaps)
        {
            YAML::Node merged = YAML::Node(YAML::NodeType::Map);
            for (const auto &d : docs)
            {
                for (auto it = d.begin(); it != d.end(); ++it)
                {
                    merged[it->first] = it->second;
                }
            }
            return merged;
        }

        // If all docs are sequences -> concatenate in read order
        bool allSeq = std::all_of(docs.begin(), docs.end(), [](const YAML::Node &n)
                                  { return n.IsSequence(); });
        if (allSeq)
        {
            YAML::Node concat = YAML::Node(YAML::NodeType::Sequence);
            for (const auto &d : docs)
            {
                for (std::size_t i = 0; i < d.size(); ++i)
                    concat.push_back(d[i]);
            }
            return concat;
        }

        // Mixed types: return a sequence containing each top-level document in read order
        YAML::Node seq = YAML::Node(YAML::NodeType::Sequence);
        for (const auto &d : docs)
            seq.push_back(d);
        return seq;
    }

    inline void resolveIncludesRec(YAML::Node &node, const std::filesystem::path &baseDir, std::unordered_set<std::string> &seen)
    {
        if (!node)
            return;

        // If node has an include tag
        std::string tag = node.Tag();
        if (!tag.empty() && (tag == "!include" || tag.find(":include") != std::string::npos))
        {
            if (!node.IsScalar())
            {
                throw std::runtime_error("!include must be applied to a scalar filename");
            }
            std::string rel = node.as<std::string>();
            auto inclPath = join_base(baseDir, rel);
            namespace fs = std::filesystem;
            try
            {
                inclPath = fs::weakly_canonical(inclPath);
            }
            catch (...)
            {
                // leave as-is
            }
            std::string inclStr = inclPath.string();
            if (inclStr.empty())
                throw std::runtime_error("empty include path");
            if (seen.count(inclStr))
            {
                throw std::runtime_error("Include cycle detected: " + inclStr);
            }
            if (!fs::exists(inclPath))
            {
                throw std::runtime_error("Included file not found: " + inclStr);
            }
            seen.insert(inclStr);
            YAML::Node included = YAML::LoadFile(inclStr);
            resolveIncludesRec(included, inclPath.parent_path(), seen);
            node = included;
            return;
        }

        if (node.IsMap())
        {
            // iterate keys and resolve their values
            for (auto it = node.begin(); it != node.end(); ++it)
            {
                YAML::Node key = it->first;
                YAML::Node val = it->second;
                resolveIncludesRec(val, baseDir, seen);
                node[key] = val;
            }
        }
        else if (node.IsSequence())
        {
            for (std::size_t i = 0; i < node.size(); ++i)
            {
                YAML::Node el = node[i];
                resolveIncludesRec(el, baseDir, seen);
                node[i] = el;
            }
        }
    }

} // namespace yaml_utils

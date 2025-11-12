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

        YAML::Node root = YAML::LoadFile(p.string());
        std::unordered_set<std::string> seen;
        seen.insert(p.string());
        resolveIncludesRec(root, p.parent_path(), seen);
        return root;
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

#pragma once

#include <vector>

namespace palette {

class Color;

class ColorKMeans {
 public:
    enum class SeedMode { keep_existing, random_spread, static_spread };

    static bool find_clusters(size_t num_clusters,
                              SeedMode seed_mode,
                              const std::vector<Color> &colors,
                              std::vector<Color> &color_centroids);
};
}  // namespace palette

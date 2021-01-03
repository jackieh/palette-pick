#pragma once

#include <vector>

namespace palette {

class Color;

class ColorKMeans {
 public:
    static bool find_clusters(size_t num_clusters,
                              const std::vector<Color> &colors,
                              std::vector<Color> &color_centroids);
};
}  // namespace palette

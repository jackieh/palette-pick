#include <vector>

#include <armadillo>

#include <Magick++.h>

#include "color.h"
#include "color_k_means.h"

namespace palette {

bool ColorKMeans::find_clusters(size_t num_clusters,
                                const std::vector<Color> &colors,
                                std::vector<Color> &color_centroids) {
    if (num_clusters >= colors.size()) {
        color_centroids = std::vector<Color>(colors);
        return true;
    }
    color_centroids.clear();
    color_centroids.reserve(num_clusters);

    arma::mat means = arma::eye(3, num_clusters);
    arma::mat data = arma::eye(3, colors.size());
    for (size_t c = 0; c < colors.size(); ++c) {
        const Color &color = colors.at(c);
        data(0, c) = static_cast<int>(color.get().quantumRed());
        data(1, c) = static_cast<int>(color.get().quantumGreen());
        data(2, c) = static_cast<int>(color.get().quantumBlue());
    }

    const bool k_means_success = arma::kmeans(
        means, data, num_clusters, arma::static_spread,
        /* num iterations */ 10, /* print mode */ false);
    for (size_t c = 0; c < num_clusters; ++c) {
        Magick::Quantum quantumRed =
            static_cast<Magick::Quantum>(means(0, c));
        Magick::Quantum quantumGreen =
            static_cast<Magick::Quantum>(means(1, c));
        Magick::Quantum quantumBlue =
            static_cast<Magick::Quantum>(means(2, c));
        color_centroids.emplace_back(Magick::Color(
                quantumRed, quantumGreen, quantumBlue));
    }
    return k_means_success;
}
}  // namespace palette

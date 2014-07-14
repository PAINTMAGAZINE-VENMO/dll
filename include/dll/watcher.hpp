//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#ifndef DBN_WATCHER_HPP
#define DBN_WATCHER_HPP

#include <fstream>

#include <sys/stat.h>

#include "stop_watch.hpp"
#include "rbm_traits.hpp"

namespace dll {

template<typename RBM>
struct default_watcher {
    stop_watch<std::chrono::seconds> watch;

    void training_begin(const RBM& rbm){
        std::cout << "RBM: Train with learning_rate=" << rbm.learning_rate;

        if(rbm_traits<RBM>::has_momentum()){
            std::cout << ", momentum=" << rbm.momentum;
        }

        if(rbm_traits<RBM>::decay() != decay_type::NONE){
            std::cout << ", weight_cost=" << rbm.weight_cost;
        }

        if(rbm_traits<RBM>::has_sparsity()){
            std::cout << ", sparsity_target=" << rbm.sparsity_target;
        }

        std::cout << std::endl;
    }

    void epoch_end(std::size_t epoch, double error, const RBM& rbm){
        printf("epoch %ld - Reconstruction error average: %.5f - Free energy: %.3f\n",
            epoch, error, rbm.free_energy());
    }

    void training_end(const RBM&){
        std::cout << "Training took " << watch.elapsed() << "s" << std::endl;
    }
};

template<typename RBM>
struct histogram_watcher {
    default_watcher<RBM> parent;

    void training_begin(const RBM& rbm){
        parent.training_begin(rbm);
    }

    void epoch_end(std::size_t epoch, double error, const RBM& rbm){
        parent.epoch_end(epoch, error, rbm);
    }

    void training_end(const RBM& rbm){
        parent.training_end(rbm);
    }

    void generate_hidden_images(size_t epoch, const RBM& rbm){
        mkdir("reports", 0777);

        auto folder = "reports/epoch_" + std::to_string(epoch);
        mkdir(folder.c_str(), 0777);

        for(size_t j = 0; j < RBM::num_hidden; ++j){
            auto path = folder + "/h_" + std::to_string(j) + ".dat";
            std::ofstream file(path, std::ios::out);

            if(!file){
                std::cout << "Could not open file " << path << std::endl;
            } else {
                size_t i = RBM::num_visible;
                while(i > 0){
                    --i;

                    auto value = rbm.w(i,j);
                    file << static_cast<size_t>(value > 0 ? static_cast<size_t>(value * 255.0) << 8 : static_cast<size_t>(-value * 255.0) << 16) << " ";
                }

                file << std::endl;
                file.close();
            }
        }
    }

    void generate_histograms(size_t epoch, const RBM& rbm){
        mkdir("reports", 0777);

        auto folder = "reports/epoch_" + std::to_string(epoch);
        mkdir(folder.c_str(), 0777);

        generate_histogram(folder + "/weights.dat", rbm.w);
        generate_histogram(folder + "/visibles.dat", rbm.a);
        generate_histogram(folder + "/hiddens.dat", rbm.b);
    }

    template<typename Container>
    void generate_histogram(const std::string& path, const Container& weights){
        std::ofstream file(path, std::ios::out);

        if(!file){
            std::cout << "Could not open file " << path << std::endl;
        } else {
            for(auto& weight : weights){
                file << weight << std::endl;
            }

            file << std::endl;
            file.close();
        }
    }
};

} //end of dbn namespace

#endif
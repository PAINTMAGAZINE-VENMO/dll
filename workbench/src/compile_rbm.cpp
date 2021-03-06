//=======================================================================
// Copyright (c) 2014-2016 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <iostream>
#include <chrono>

#include "dll/rbm.hpp"
#include "dll/dbn.hpp"

#include "mnist/mnist_reader.hpp"
#include "mnist/mnist_utils.hpp"

// 5 3-layer networks

int main(int, char**) {
    auto dataset = mnist::read_dataset_direct<std::vector, etl::dyn_vector<float>>();

#define decl_dbn3(NAME, NAME_T, F) \
    using NAME_T = \
        dll::dbn_desc< \
            dll::dbn_layers< \
                dll::rbm_desc<28*28, 100 + F, dll::momentum>::layer_t, \
                dll::rbm_desc<100 + F, 200 + F, dll::momentum>::layer_t, \
                dll::rbm_desc<200 + F, 10, dll::momentum, dll::hidden<dll::unit_type::SOFTMAX>>::layer_t>>::dbn_t; \
    auto NAME = std::make_unique<NAME_T>(); \
    NAME->pretrain(dataset.training_images, 20);

    decl_dbn3(dbn1,dbn1_t,1)
    decl_dbn3(dbn2,dbn2_t,2)
    decl_dbn3(dbn3,dbn3_t,3)
    decl_dbn3(dbn4,dbn4_t,4)
    decl_dbn3(dbn5,dbn5_t,5)

    return 0;
}

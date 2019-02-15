#define BOOST_TEST_MODULE Factored_MDP_CooperativeRLModel
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <AIToolbox/Utils/Core.hpp>
#include <AIToolbox/Factored/MDP/CooperativeRLModel.hpp>

#include "Utils/SysAdmin.hpp"

namespace ai = AIToolbox;
namespace aif = AIToolbox::Factored;
namespace afm = AIToolbox::Factored::MDP;

BOOST_AUTO_TEST_CASE( construction ) {
    auto model = makeSysAdminBiRing(7, 0.1, 0.2, 0.3, 0.4, 0.2, 0.2, 0.1);

    afm::CooperativeExperience exp(model.getS(), model.getA(), model.getTransitionFunction().nodes);
    afm::CooperativeRLModel rl(exp, 0.9);

    const auto & tt = model.getTransitionFunction();
    const auto & t = rl.getTransitionFunction();
    const auto & r = rl.getRewardFunction();

    BOOST_CHECK_EQUAL(rl.getDiscount(), 0.9);
    BOOST_CHECK_EQUAL(ai::veccmp(model.getS(), rl.getS()), 0);
    BOOST_CHECK_EQUAL(ai::veccmp(model.getA(), rl.getA()), 0);

    BOOST_CHECK_EQUAL(tt.nodes.size(), t.nodes.size());
    // Note that the learned reward function has a different format from the
    // original model (vector<vector<Vector>> vs FactoredMatrix2D). This means
    // we cannot compare them directly.
    BOOST_CHECK_EQUAL(tt.nodes.size(), r.size());

    for (size_t i = 0; i < t.nodes.size(); ++i) {
        BOOST_CHECK_EQUAL(ai::veccmp(tt[i].actionTag, t.nodes[i].actionTag), 0);

        BOOST_CHECK_EQUAL(tt[i].nodes.size(), t.nodes[i].nodes.size());
        BOOST_CHECK_EQUAL(tt[i].nodes.size(), r[i].size());

        for (size_t j = 0; j < t.nodes[i].nodes.size(); ++j) {
            const auto & ttn = tt.nodes[i].nodes[j];
            const auto & tn = t.nodes[i].nodes[j];

            BOOST_CHECK_EQUAL(ai::veccmp(ttn.tag, tn.tag), 0);

            BOOST_CHECK_EQUAL(ttn.matrix.rows(), tn.matrix.rows());
            BOOST_CHECK_EQUAL(ttn.matrix.cols(), tn.matrix.cols());

            BOOST_CHECK_EQUAL(ttn.matrix.rows(), r[i][j].size());

            for (int x = 0; x < tn.matrix.rows(); ++x) {
                BOOST_CHECK_EQUAL(tn.matrix(x, 0), 1.0);
                BOOST_CHECK_EQUAL(r[i][j][x], 0);
                for (int y = 1; y < tn.matrix.cols(); ++y)
                    BOOST_CHECK_EQUAL(tn.matrix(x, y), 0.0);
            }
        }
    }
}

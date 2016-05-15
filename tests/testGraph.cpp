//----------------------------------------------------------------------------------
//
// SAFECASS: Safety Architecture For Engineering Computer-Assisted Surgical Systems
//
// Copyright (C) 2014-2016 Min Yang Jung and Peter Kazanzides
//
//----------------------------------------------------------------------------------
//
// Created on   : May 14, 2016
// Last revision: May 14, 2016
// Author       : Min Yang Jung <myj@jhu.edu>
// Github       : https://github.com/safecass/safecass
//
#include "gtest/gtest.h"
#include "config.h"
#include <boost/graph/graphviz.hpp>

//using namespace SC;

// Examples from Boost.Graph
//
// More GraphViz examples are available: http://www.graphviz.org/Gallery.php
TEST(BoostGraphDemo, GraphVizWrite)
{
    enum files_e { dax_h, yow_h, boz_h, zow_h, foo_cpp,
                foo_o, bar_cpp, bar_o, libfoobar_a,
                zig_cpp, zig_o, zag_cpp, zag_o,
                    libzigzag_a, killerapp, N };
    const char* name[] = { "dax.h", "yow.h", "boz.h", "zow.h", "foo.cpp",
                        "foo.o", "bar.cpp", "bar.o", "libfoobar.a",
                        "zig.cpp", "zig.o", "zag.cpp", "zag.o",
                        "libzigzag.a", "killerapp" };

    typedef std::pair<int,int> Edge;
    Edge used_by[] = {
        Edge(dax_h, foo_cpp), Edge(dax_h, bar_cpp), Edge(dax_h, yow_h),
        Edge(yow_h, bar_cpp), Edge(yow_h, zag_cpp),
        Edge(boz_h, bar_cpp), Edge(boz_h, zig_cpp), Edge(boz_h, zag_cpp),
        Edge(zow_h, foo_cpp),
        Edge(foo_cpp, foo_o),
        Edge(foo_o, libfoobar_a),
        Edge(bar_cpp, bar_o),
        Edge(bar_o, libfoobar_a),
        Edge(libfoobar_a, libzigzag_a),
        Edge(zig_cpp, zig_o),
        Edge(zig_o, libzigzag_a),
        Edge(zag_cpp, zag_o),
        Edge(zag_o, libzigzag_a),
        Edge(libzigzag_a, killerapp)
    };
    const int nedges = sizeof(used_by)/sizeof(Edge);
    int weights[nedges];
    std::fill(weights, weights + nedges, 1);

    using namespace boost;

    typedef adjacency_list< vecS, vecS, directedS,
            property< vertex_color_t, default_color_type >,
            property< edge_weight_t, int >
                > Graph;
    Graph g(used_by, used_by + nedges, weights, N);

    write_graphviz(std::cout, g, make_label_writer(name));
}

// The following tutorial example generates linker error on GCC:
// https://travis-ci.org/safecass/safecass/jobs/130288140
#if 0
TEST(BoostGraphDemo, GraphVizReadWrite)
{
    using namespace boost;

    typedef boost::adjacency_list<vecS, vecS, directedS,
                                property<vertex_name_t, std::string>,
                                property<edge_weight_t, double> > Digraph;

    typedef boost::adjacency_list<vecS, vecS, undirectedS,
                                property<vertex_name_t, std::string>,
                                property<edge_weight_t, double> > Graph;

    const std::string filename(SC_SOURCE_ROOT_DIR"/tests/data/graphviz-example.dot");

    std::ifstream in(filename.c_str());

    Graph g;
    dynamic_properties dp;
    dp.property("id", get(vertex_name, g));
    dp.property("weight", get(edge_weight, g));
    EXPECT_TRUE(read_graphviz(in, g, dp, "id"));

    EXPECT_TRUE(num_vertices(g) == 4);
    EXPECT_TRUE(num_edges(g) == 4);

    typedef graph_traits<Graph>::vertex_descriptor Vertex;

    std::map<std::string, Vertex> name_to_vertex;
    BGL_FORALL_VERTICES(v, g, Graph)
        name_to_vertex[get(vertex_name, g, v)] = v;

    // Check vertices
    EXPECT_TRUE(name_to_vertex.find("0") != name_to_vertex.end());
    EXPECT_TRUE(name_to_vertex.find("1") != name_to_vertex.end());
    EXPECT_TRUE(name_to_vertex.find("foo") != name_to_vertex.end());
    EXPECT_TRUE(name_to_vertex.find("bar") != name_to_vertex.end());

    // Check edges
    EXPECT_TRUE(edge(name_to_vertex["0"], name_to_vertex["1"], g).second);
    EXPECT_TRUE(edge(name_to_vertex["1"], name_to_vertex["foo"], g).second);
    EXPECT_TRUE(edge(name_to_vertex["foo"], name_to_vertex["bar"], g).second);
    EXPECT_TRUE(edge(name_to_vertex["1"], name_to_vertex["bar"], g).second);

    EXPECT_EQ(3.14159, get(edge_weight, g, edge(name_to_vertex["0"], name_to_vertex["1"], g).first));
    EXPECT_EQ(2.71828, get(edge_weight, g, edge(name_to_vertex["1"], name_to_vertex["foo"], g).first));
    EXPECT_EQ(10.0, get(edge_weight, g, edge(name_to_vertex["foo"], name_to_vertex["bar"], g).first));
    EXPECT_EQ(10.0, get(edge_weight, g, edge(name_to_vertex["1"], name_to_vertex["bar"], g).first));

    // Write out the graph
    write_graphviz_dp(std::cout, g, dp, std::string("id"));
}
#endif

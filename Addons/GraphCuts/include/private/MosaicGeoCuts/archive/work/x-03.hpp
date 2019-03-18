template <class ImageIn, class ImageGrad, class ImageMarker, class SE,
          class ImageOut>
RES_T GeoCuts_MinSurfaces_with_Line(const ImageIn &imIn,
                                      const ImageGrad &imGrad,
                                      const ImageMarker &imMarker, const SE &nl,
                                      ImageOut &imOut)
{
  MORPHEE_ENTER_FUNCTION("t_GeoCuts_MinSurfaces_With_Line");

  std::cout << "Enter function t_GeoCuts_MinSurfaces_With_Line" << std::endl;

  if ((!imOut.isAllocated())) {
    MORPHEE_REGISTER_ERROR("Not allocated");
    return RES_NOT_ALLOCATED;
  }

  if ((!imIn.isAllocated())) {
    MORPHEE_REGISTER_ERROR("Not allocated");
    return RES_NOT_ALLOCATED;
  }

  if ((!imGrad.isAllocated())) {
    MORPHEE_REGISTER_ERROR("Not allocated");
    return RES_NOT_ALLOCATED;
  }

  if ((!imMarker.isAllocated())) {
    MORPHEE_REGISTER_ERROR("Not allocated");
    return RES_NOT_ALLOCATED;
  }

  // common image iterator
  typename ImageIn::const_iterator it, iend;
  morphee::selement::Neighborhood<SE, ImageIn> neighb(imIn, nl);
  typename morphee::selement::Neighborhood<SE, ImageIn>::iterator nit, nend;
  offset_t o0;
  offset_t o1;

  // needed for max flow: capacit map, rev_capacity map, etc.
  typedef boost::adjacency_list_traits<boost::vecS, boost::vecS,
                                       boost::directedS>
      Traits;
  typedef boost::adjacency_list<
      boost::listS, boost::vecS, boost::directedS,
      boost::property<boost::vertex_name_t, std::string>,
      boost::property<
          boost::edge_capacity_t, double,
          boost::property<
              boost::edge_residual_capacity_t, double,
              boost::property<boost::edge_reverse_t, Traits::edge_descriptor>>>>
      Graph_d;

  Graph_d g;

  double sigma = 1.0;

  boost::property_map<Graph_d, boost::edge_capacity_t>::type capacity =
      boost::get(boost::edge_capacity, g);

  boost::property_map<Graph_d, boost::edge_reverse_t>::type rev =
      get(boost::edge_reverse, g);

  boost::property_map<Graph_d, boost::edge_residual_capacity_t>::type
      residual_capacity = get(boost::edge_residual_capacity, g);

  bool in1;
  Graph_d::edge_descriptor e1, e2, e3, e4;
  Graph_d::vertex_descriptor vSource, vSink;
  int numVert = 0;

  std::cout << "build Region Adjacency Graph" << std::endl;

  for (it = imIn.begin(), iend = imIn.end(); it != iend; ++it) {
    // for all pixels in imIn create a vertex
    o0      = it.getOffset();
    int val = imIn.pixelFromOffset(o0);

    if (val > numVert) {
      numVert = val;
    }
  }

  std::cout << "build Region Adjacency Graph Vertices :" << numVert
            << std::endl;

  for (int i = 0; i <= numVert; i++) {
    boost::add_vertex(g);
  }

  vSource = boost::add_vertex(g);
  vSink   = boost::add_vertex(g);

  std::cout << "build Region Adjacency Graph Edges" << std::endl;

  for (it = imIn.begin(), iend = imIn.end(); it != iend; ++it) {
    o1 = it.getOffset();
    // std::cout<<o1<<std::endl;

    int val    = imIn.pixelFromOffset(o1);
    int marker = imMarker.pixelFromOffset(o1);

    if (marker == 2) {
      boost::tie(e4, in1) = boost::edge(vSource, val, g);
      if (in1 == 0) {
        // std::cout<<"Add new edge marker 2"<<std::endl;
        boost::tie(e4, in1) = boost::add_edge(vSource, val, g);
        boost::tie(e3, in1) = boost::add_edge(val, vSource, g);
        capacity[e4]        = (std::numeric_limits<double>::max)();
        capacity[e3]        = (std::numeric_limits<double>::max)();
        rev[e4]             = e3;
        rev[e3]             = e4;
      }
    } else if (marker == 3) {
      boost::tie(e3, in1) = boost::edge(vSink, val, g);
      if (in1 == 0) {
        // std::cout<<"Add new edge marker 3"<<std::endl;
        boost::tie(e4, in1) = boost::add_edge(val, vSink, g);
        boost::tie(e3, in1) = boost::add_edge(vSink, val, g);
        capacity[e4]        = (std::numeric_limits<double>::max)();
        capacity[e3]        = (std::numeric_limits<double>::max)();
        rev[e4]             = e3;
        rev[e3]             = e4;
      }
    }

    neighb.setCenter(o1);

    for (nit = neighb.begin(), nend = neighb.end(); nit != nend; ++nit) {
      const offset_t o2 = nit.getOffset();
      int val2          = imIn.pixelFromOffset(o2);

      if (o2 > o1) {
        if (val != val2) {
          boost::tie(e3, in1) = boost::edge(val, val2, g);
          // std::cout<<in1<<std::endl;
          // std::cout<<"Compute Gradient"<<std::endl;
          double val3 = imGrad.pixelFromOffset(o1);
          double val4 = imGrad.pixelFromOffset(o2);
          double maxi = std::max(val3, val4);
          double cost = 10000000.0 / (1.0 + std::pow(maxi, 5));

          if (in1 == 0) {
            // std::cout<<"Add new edge"<<std::endl;
            boost::tie(e4, in1) = boost::add_edge(val, val2, g);
            boost::tie(e3, in1) = boost::add_edge(val2, val, g);
            capacity[e4]        = cost;
            capacity[e3]        = cost;
            rev[e4]             = e3;
            rev[e3]             = e4;
          } else {
            // std::cout<<"existing edge"<<std::endl;
            boost::tie(e4, in1) = boost::edge(val, val2, g);
            boost::tie(e3, in1) = boost::edge(val2, val, g);
            capacity[e4]        = capacity[e4] + cost;
            capacity[e3]        = capacity[e3] + cost;
          }
        }
      }
    }
  }

  boost::property_map<Graph_d, boost::vertex_index_t>::type indexmap =
      boost::get(boost::vertex_index, g);
  std::vector<boost::default_color_type> color(boost::num_vertices(g));

  std::cout << "Compute Max flow" << std::endl;
#if BOOST_VERSION >= 104700
  double flow = boykov_kolmogorov_max_flow(g, capacity, residual_capacity, rev,
                                           &color[0], indexmap, vSource, vSink);
#else
  double flow = kolmogorov_max_flow(g, capacity, residual_capacity, rev,
                                    &color[0], indexmap, vSource, vSink);
#endif
  std::cout << "c  The total flow:" << std::endl;
  std::cout << "s " << flow << std::endl << std::endl;

  for (it = imIn.begin(), iend = imIn.end(); it != iend; ++it) {
    // for all pixels in imIn create a vertex and an edge
    o1      = it.getOffset();
    int val = imIn.pixelFromOffset(o1);

    if (color[val] == color[vSource])
      imOut.setPixel(o1, 2);
    else if (color[val] == color[vSink])
      imOut.setPixel(o1, 3);
    else
      imOut.setPixel(o1, 4);
  }

  for (it = imIn.begin(), iend = imIn.end(); it != iend; ++it) {
    // for all pixels in imIn create a vertex and an edge
    o1           = it.getOffset();
    int valeur_c = imOut.pixelFromOffset(o1);
    neighb.setCenter(o1);

    for (nit = neighb.begin(), nend = neighb.end(); nit != nend; ++nit) {
      const offset_t o2 = nit.getOffset();
      int valeur_n      = imOut.pixelFromOffset(o2);

      if (valeur_n != valeur_c && valeur_n != 0 && valeur_c != 0) {
        imOut.setPixel(o1, 0);
      }
    }
  }

  return RES_OK;
}

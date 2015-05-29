#include <iostream>
#include <assert.h>
#include <vector>

#include "chooseser.h"
#include "libjpcnn.h"
#include "graph.h"
#include "convnode.h"
#include "relunode.h"
#include "poolnode.h"
#include "neuronnode.h"
#include "maxnode.h"
#include "buffer.h"
#include "prepareinput.h"
#include "math.h"
#include "matrix_ops.h"

Buffer* buffer_from_ocarray(const Array<real_4>& array, const std::vector<int32_t> dim) {
  int32_t dimensionsCount = dim.size();
  const int32_t *dimensions = &dim[0];
  Dimensions dims(dimensions, dimensionsCount);
  assert(array.length() == dims.elementCount());

  Buffer*  buffer = new Buffer(dims);
  memcpy(buffer->_data, array.data(), array.length() * sizeof(real_4));

  return buffer;
}


Graph* new_graph_from_scratch(){
  // manually create a network. see reference: graph.cpp:new_graph_from_file()
  // Network architecture:
  // [conv - relu - pool] - [conv - relu - pool]  - fc - relu - fc - softmax
  Graph* graph = new Graph();


  Val params, mean;
  LoadValFromFile("data/mnist_model_reshaped.p", params, SERIALIZE_P0);
  LoadValFromFile("data/mean_img.p", mean, SERIALIZE_P0);

  graph->_useMemoryMap = false;
  //graph->_isHomebrewed = isHomebrewed; // TODO

  graph->_inputSize = 28;
  graph->_dataMean = buffer_from_ocarray(mean, {28*28});

  
  /**********************
   * output labels: digit 0-9
   **********************/
  graph->_labelNamesLength = 10;
  graph->_labelNames = (char**)(malloc(sizeof(char*) * graph->_labelNamesLength));
  for(size_t i; i < 10; i++){
    graph->_labelNames[i] = (char*)(malloc(10));
    sprintf(graph->_labelNames[i], "%lu", i);
  }	
  
  /**********************
   * layers
   **********************/
  int numberOfLayers = 10;
  graph->_layersLength = numberOfLayers;
  graph->_layers = (BaseNode**)(malloc(sizeof(BaseNode*) * numberOfLayers));	
  
  // layer 0: conv1, window 5x5, k = 8
  // reference: convnode.cpp:new_convnode_from_tag()
  ConvNode* layer0 = new ConvNode();
  layer0->_kernelCount = 8;
  layer0->_kernelWidth = 5;
  layer0->_sampleStride = 1; //TODO
  layer0->_kernels = buffer_from_ocarray(params["W1"], {5*5*1, 8});
  layer0->_useBias = true;
  layer0->_bias = buffer_from_ocarray(params["b1"], {8});
  layer0->_marginSize = 2;
  layer0->setName("conv1");
  graph->_layers[0] = layer0;
  
  // layer 1: relu1
  ReluNode* layer1 = new ReluNode();
  layer1->setName("relu1");
  graph->_layers[1] = layer1;

  // layer 2: pool1, window 2x2, stride 2
  PoolNode* layer2 = new PoolNode();
  layer2->_patchWidth = 2;
  layer2->_stride = 2;
  layer2->_mode = PoolNode::EModeMax;
  layer2->setName("pool1");
  graph->_layers[2] = layer2;

  // layer 3: conv2, window 5x5, k = 16
  // reference: convnode.cpp:new_convnode_from_tag()
  ConvNode* layer3 = new ConvNode();
  layer3->_kernelCount = 16;
  layer3->_kernelWidth = 5;
  layer3->_sampleStride = 1; //TODO
  layer3->_kernels = buffer_from_ocarray(params["W2"], {5*5*8, 16});
  layer3->_useBias = true;
  layer3->_bias = buffer_from_ocarray(params["b2"], {16});
  layer3->_marginSize = 2;
  layer3->setName("conv2");
  graph->_layers[3] = layer3;
  
  // layer 4: relu2
  ReluNode* layer4 = new ReluNode();
  layer4->setName("relu2");
  graph->_layers[4] = layer4;

  // layer 5: pool2, window 2x2, stride 2
  PoolNode* layer5 = new PoolNode();
  layer5->_patchWidth = 2;
  layer5->_stride = 2;
  layer5->_mode = PoolNode::EModeMax;
  layer5->setName("pool2");
  graph->_layers[5] = layer5;

  // layer 6: fc1, k=128
  NeuronNode* layer6 = new NeuronNode();
  layer6->_outputsCount = 128;
  layer6->_weights = buffer_from_ocarray(params["W3"], {7*7*16, 128});
  layer6->_useBias = true;
  layer6->_bias = buffer_from_ocarray(params["b3"], {128}); 
  layer6->setName("fc1");
  graph->_layers[6] = layer6;

  // layer 7: relu3
  ReluNode* layer7 = new ReluNode();
  layer7->setName("relu3");
  graph->_layers[7] = layer7;

  // layer 8: fc2, k=10
  NeuronNode* layer8 = new NeuronNode();
  layer8->_outputsCount = 10;
  layer8->_weights = buffer_from_ocarray(params["W4"], {128, 10});
  layer8->_useBias = true;
  layer8->_bias = buffer_from_ocarray(params["b4"], {10}); 
  layer8->setName("fc2");
  graph->_layers[8] = layer8;

  // layer 9: softmax
  MaxNode* layer9 = new MaxNode();
  layer9->setName("softmax");
  graph->_layers[9] = layer9;

  return graph;
}

struct ConvNetFeatures
{
  float* features;
  int length;
};

int main(int argc, const char * argv[]) {

  //Graph* scratch_graph = new_graph_from_scratch();
  //save_graph_to_file(scratch_graph, "data/mdnet.ntwk");
  Graph* graph = new_graph_from_file("data/mdnet.ntwk", false, false);
  
  //graph->printDebugOutput();

  // load image
  Val var_img, inter_img;
  LoadValFromFile("data/ori_test_img5.p", var_img, SERIALIZE_P0);
  //LoadValFromFile("data/inter_img.p", inter_img, SERIALIZE_P0);
  Buffer *img = buffer_from_ocarray(var_img, {1, 28, 28, 1});

  bool useAPI = true;
  if (!useAPI) {
    /*********************
     * use internal subroutines
     **********************/

    // substract mean
    matrix_add_inplace(img, graph->_dataMean, -1.0f);
    // run cnn
    Buffer *predictions = graph->run(img, 0);
    // print results
    for(int i = 0; i < 10; i++)
      cout << graph->_labelNames[i] << ": " << predictions->_data[i] <<endl;

  } else {
    /*********************
     * use libjpcnn API
     **********************/

    ConvNetFeatures features = {.features = nullptr, .length = 0};
    char** predictionsLabels;
    int predictionsLabelsLength;
    jpcnn_classify_image(graph,
                         img,
                         JPCNN_SKIP_RESCALE,
                         0,
                         &(features.features),
                         &(features.length),
                         &predictionsLabels,
                         &predictionsLabelsLength);
    // print results
    for(int i = 0; i < predictionsLabelsLength; i++)
      cout << predictionsLabels[i] << ": " << features.features[i] <<endl;
  }


  delete graph;

  return 0;
}

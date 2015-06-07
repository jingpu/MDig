//
//  graph.cpp
//  jpcnn
//
//  Created by Peter Warden on 1/9/14.
//  Copyright (c) 2014 Jetpac, Inc. All rights reserved.
//

#include "graph.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

#include "buffer.h"
#include "binary_format.h"
#include "basenode.h"
#include "nodefactory.h"

#if __APPLE__
  #include "TargetConditionals.h"
  #if TARGET_OS_IPHONE
    #define USE_BUNDLE_LOADING
  #endif // TARGET_OS_IPHONE
#endif // __APPLE__

#ifdef USE_BUNDLE_LOADING
#import <UIKit/UIKit.h>
#endif // USE_BUNDLE_LOADING

//#define DO_LOG_OPERATIONS
//#define CHECK_RESULTS
//#define SAVE_RESULTS
#if defined(CHECK_RESULTS) || defined(SAVE_RESULTS)
#define FN_LEN (1024)
#define DUMP_FILE_PATH ("data/libccv_blobs/")
#endif // CHECK_RESULTS || SAVE_RESULTS

Graph::Graph() :
  _useMemoryMap(false),
  _isHomebrewed(false),
  _isLibCCV(false),
  _source("none"),
  _inputSize(256),
  _fileTag(NULL),
  _dataMean(NULL),
  _preparationNode(NULL),
  _layers(NULL),
  _layersLength(0),
  _labelNames(NULL),
  _labelNamesLength(0) {
}

Graph::~Graph() {
  if (_fileTag != NULL) {
    deallocate_file_tag(_fileTag, _useMemoryMap);
  }
  if (_dataMean != NULL) {
    delete _dataMean;
  }
  if (_preparationNode != NULL) {
    delete _preparationNode;
  }
  if (_layers != NULL) {
    for (int index = 0; index < _layersLength; index += 1) {
      delete _layers[index];
    }
    free(_layers);
  }
  if (_labelNames != NULL) {
    for (int index = 0; index < _labelNamesLength; index += 1) {
      free(_labelNames[index]);
    }
    free(_labelNames);
  }
}

Buffer* Graph::run(Buffer* input, int layerOffset) {

#ifdef DO_LOG_OPERATIONS
  fprintf(stderr, "Graph::run() input=%s\n", input->debugString());
#endif // DO_LOG_OPERATIONS

  
  const int howManyLayers = (_layersLength + layerOffset);
  const int batchingLayers =  6;
  
  const Dimensions inputDims = input->_dims;
  const int imageCount = inputDims[0];
  const int inputWidth = inputDims[2];
  const int inputHeight = inputDims[1];
  const int inputChannels = inputDims[3];

  const Dimensions singleImageDims(1, inputHeight, inputWidth, inputChannels);
  const size_t valuesPerImage = inputHeight * inputWidth * inputChannels;
  const size_t bytesPerImage = valuesPerImage * sizeof(jpfloat_t);
  
  // prepare buffer for holding batched images
  const int batchedWidth = 7;
  const int batchedHeight = 7;
  const int batchedChannels = 16;
  const Dimensions batchedDims(imageCount, batchedHeight, batchedWidth, batchedChannels);
  const size_t valuesPerBatchedImage = batchedHeight * batchedWidth * batchedChannels;
  const size_t bytesPerBatchedImage = valuesPerBatchedImage * sizeof(jpfloat_t);
  Buffer *batchedBuffer = new Buffer(batchedDims);
  jpfloat_t* desData = batchedBuffer->_data;

  // pass images to conv layers one by one
  for (int imageIndex = 0; imageIndex < imageCount; imageIndex++ ){
    Buffer* currentInput = new Buffer(singleImageDims);
    const jpfloat_t* inputData = input->_data + valuesPerImage * imageIndex;
    memcpy(currentInput->_data, inputData, bytesPerImage);

    for (int index = 0; index < batchingLayers; index += 1) {
      BaseNode* layer = _layers[index];

      //#ifdef DO_LOG_OPERATIONS
      struct timeval start;
      gettimeofday(&start, NULL);
      //#endif // DO_LOG_OPERATIONS
      
      Buffer* currentOutput = layer->run(currentInput);
      currentOutput->setName(layer->_name);
      
      //#ifdef DO_LOG_OPERATIONS
      struct timeval end;
      gettimeofday(&end, NULL);
      //fprintf(stderr, "Graph::run() currentOutput=%s\n", currentOutput->debugString());
      
      long seconds  = end.tv_sec  - start.tv_sec;
      long useconds = end.tv_usec - start.tv_usec;
      long duration = ((seconds) * 1000000 + useconds) ;
      fprintf(stderr, "Layer %d took %ldus\n", index, duration);
      //#endif // DO_LOG_OPERATIONS
      
      currentInput = currentOutput;
    }
    assert(currentInput->_dims._length == 4);
    assert(currentInput->_dims[0] == 1);
    assert(currentInput->_dims[1] == batchedHeight);
    assert(currentInput->_dims[2] == batchedWidth);
    assert(currentInput->_dims[3] == batchedChannels);

    const jpfloat_t* srcData = currentInput->_data;
    memcpy(desData, srcData, bytesPerBatchedImage);
    desData += valuesPerBatchedImage;
  }

  // batch images at FC layers
  Buffer* currentInput = batchedBuffer;
  for (int index = batchingLayers; index < howManyLayers; index += 1) {
    BaseNode* layer = _layers[index];
      
    //#ifdef DO_LOG_OPERATIONS
    struct timeval start;
    gettimeofday(&start, NULL);
    //#endif // DO_LOG_OPERATIONS

    Buffer* currentOutput = layer->run(currentInput);
    currentOutput->setName(layer->_name);
      
    //#ifdef DO_LOG_OPERATIONS
    struct timeval end;
    gettimeofday(&end, NULL);
    //fprintf(stderr, "Graph::run() currentOutput=%s\n", currentOutput->debugString());
      
    long seconds  = end.tv_sec  - start.tv_sec;
    long useconds = end.tv_usec - start.tv_usec;
    long duration = ((seconds) * 1000000 + useconds) ;
    fprintf(stderr, "Layer %d took %ldus\n", index, duration);
    //#endif // DO_LOG_OPERATIONS
    currentInput = currentOutput;
  }

  return currentInput;
}

void Graph::printDebugOutput() {
  fprintf(stderr, "************************\n");
  fprintf(stderr, "JPCNN Network with %d layers, input size: %u\n", _layersLength, _inputSize);
  for (int index = 0; index < _layersLength; index += 1) {
    BaseNode* layer = _layers[index];
    fprintf(stderr, "%s\n", layer->debugString());
  }
  fprintf(stderr, "************************\n");
}

Graph* new_graph_from_file(const char* filename, int useMemoryMap, int isHomebrewed) {

  SBinaryTag* graphDict = read_tag_from_file(filename, useMemoryMap);
  if (graphDict == NULL) {
    fprintf(stderr, "new_graph_from_file(): Couldn't interpret data from '%s'\n", filename);
    return NULL;
  }

  Graph* result = new Graph();

  result->_useMemoryMap = useMemoryMap;
  result->_isHomebrewed = isHomebrewed;
  result->_fileTag = graphDict;

  if (get_tag_from_dict(graphDict, "source")) {
    result->_source = get_string_from_dict(graphDict, "source");
    if (strcmp(result->_source, "libccv") == 0) {
      result->_isLibCCV = true;
    }
  }

  if (get_tag_from_dict(graphDict, "input_size")) {
    result->_inputSize = get_uint_from_dict(graphDict, "input_size");
  }

  SBinaryTag* dataMeanTag = get_tag_from_dict(graphDict, "data_mean");
  assert(dataMeanTag != NULL);
  result->_dataMean = buffer_from_tag_dict(dataMeanTag, useMemoryMap);
  assert(result->_dataMean != NULL);

  SBinaryTag* layersTag = get_tag_from_dict(graphDict, "layers");
  assert(layersTag != NULL);
  result->_layersLength = count_list_entries(layersTag);
  result->_layers = (BaseNode**)(malloc(sizeof(BaseNode*) * result->_layersLength));

  int index = 0;
  SBinaryTag* currentLayerTag = get_first_list_entry(layersTag);
  while (currentLayerTag != NULL) {
    BaseNode* layerNode = new_node_from_tag(currentLayerTag, useMemoryMap);
    result->_layers[index] = layerNode;
    index += 1;
    currentLayerTag = get_next_list_entry(layersTag, currentLayerTag);
  }

  SBinaryTag* labelNamesTag = get_tag_from_dict(graphDict, "label_names");

  result->_labelNamesLength = count_list_entries(labelNamesTag);
  result->_labelNames = (char**)(malloc(sizeof(char*) * result->_labelNamesLength));

  index = 0;
  SBinaryTag* currentLabelNameTag = get_first_list_entry(labelNamesTag);
  while (currentLabelNameTag != NULL) {
    const char* const fileLabelName = currentLabelNameTag->payload.jpchar;
    const size_t labelNameLength = strlen(fileLabelName);
    result->_labelNames[index] = (char*)(malloc(labelNameLength + 1));
    strncpy(result->_labelNames[index], fileLabelName, labelNameLength);
    result->_labelNames[index][labelNameLength] = '\0';
    index += 1;
    currentLabelNameTag = get_next_list_entry(labelNamesTag, currentLabelNameTag);
  }

  return result;
}

void save_graph_to_file(const Graph* graph, const char* filename) {

  SBinaryTag* graphDict = create_dict_tag();

  SBinaryTag* dataMeanTag = buffer_to_tag_dict(graph->_dataMean);
  graphDict = add_tag_to_dict(graphDict, "data_mean", dataMeanTag);
  free(dataMeanTag);

  SBinaryTag* inputSizeTag = create_uint_tag(graph->_inputSize);
  graphDict = add_tag_to_dict(graphDict, "input_size", inputSizeTag);
  free(inputSizeTag);

  SBinaryTag* layersTag = create_list_tag();
  for (int index = 0; index < graph->_layersLength; index += 1) {
    BaseNode* node = graph->_layers[index];
    SBinaryTag* currentLayerTag = node->toTag();
    layersTag = add_tag_to_list(layersTag, currentLayerTag);
    free(currentLayerTag);
  }
  graphDict = add_tag_to_dict(graphDict, "layers", layersTag);
  free(layersTag);

  SBinaryTag* labelNamesTag = create_list_tag();
  for (int index = 0; index < graph->_labelNamesLength; index += 1) {
    labelNamesTag = add_string_to_list(labelNamesTag, graph->_labelNames[index]);
  }
  graphDict = add_tag_to_dict(graphDict, "label_names", labelNamesTag);
  free(labelNamesTag);

  SBinaryTag* copyrightTag = create_string_tag("Copyright Jetpac Inc., 2014");
  graphDict = add_tag_to_dict(graphDict, "copyright", copyrightTag);
  free(copyrightTag);

  FILE* outputFile = fopen(filename, "wb");
  assert(outputFile != NULL);
  fwrite(graphDict, (graphDict->length + 8), 1, outputFile);
  fclose(outputFile);
  free(graphDict);
}

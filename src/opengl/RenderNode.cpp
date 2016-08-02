/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RenderNode.cpp
 * Author: bbyington
 * 
 * Created on August 2, 2016, 7:53 AM
 */

#include "RenderNode.h"

#include <cassert>
#include <iostream>
#include <numeric>

namespace ShapeShifter {
namespace Opengl {

RenderNode::RenderNode() {
}

RenderNode::~RenderNode() {
}

void RenderNode::AddChild(std::shared_ptr<RenderNode> child) {
	children.push_back(child);
	child_sizes.push_back(child->vertexCount());
	size += child->vertexCount();
}

void RenderNode::TreePopulateColor(float* rawData, size_t size) {
	size_t start = 0;
	for (const auto& child : children) {
		child->TreePopulateColor(rawData+start, child->vertexCount()*3);
		start += child->vertexCount()*3;
	}

	fillColors(rawData+start, vertexCount()*3);
	start += vertexCount()*3;
	std::cerr << start << " " << size << std::endl;
	assert(start == size);
}

//TODO unify with TreePopulatColor?  Lots of copy paste...
void RenderNode::TreePopulateVertex(float* rawData, size_t size) {
	size_t start = 0;
	for (const auto& child : children) {
		//TODO fixx all they *3 running around now...  We have two different notions of size
		child->TreePopulateVertex(rawData+start, child->vertexCount()*3);
		start += child->vertexCount()*3;
	}

	fillVertex(rawData+start, vertexCount()*3);
	start += vertexCount()*3;
	assert(start == size);
}

void RenderNode::UpdateData() {

  std::vector<float> tri_vert(size*3, 0);
	std::vector<float> tri_col(size*3, 0);
	std::cerr << tri_vert.size() << std::endl;
	
	TreePopulateVertex(tri_vert.data(), tri_vert.size());
	TreePopulateColor(tri_col.data(), tri_col.size());

  GLuint points_vbo = 0;
  glGenBuffers (1, &points_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
  glBufferData (GL_ARRAY_BUFFER, tri_vert.size() * sizeof (float), &tri_vert[0], GL_STATIC_DRAW);

	GLuint colours_vbo = 0;
  glGenBuffers (1, &colours_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, colours_vbo);
  glBufferData (GL_ARRAY_BUFFER, tri_vert.size() * sizeof (float), &tri_col[0], GL_STATIC_DRAW);

  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);
  glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer (GL_ARRAY_BUFFER, colours_vbo);
  glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
	glEnableVertexAttribArray (0);
  glEnableVertexAttribArray (1);
}

void RenderNode::RenderTree() const {
		glBindVertexArray (vao);
    glDrawArrays (GL_TRIANGLE_STRIP, 0, size);
}



//TODO this is ugly don't do this in constructor
SquareTest2D::SquareTest2D() {size = 4;}
SquareTest2D::~SquareTest2D() {}
size_t SquareTest2D::vertexCount() const { return 4; }

void SquareTest2D::fillColors(float* rawData, size_t size) {
	rawData[0] = 1.0;
	rawData[1] = 0.0;
	rawData[2] = 0.0;
	
	rawData[3] = 1.0;
	rawData[4] = 1.0;
	rawData[5] = 0.0;

	rawData[6] = 0.0;
	rawData[7] = 0.0;
	rawData[8] = 1.0;
	
	rawData[9] = 0.0;
	rawData[10] = 1.0;
	rawData[11] = 0.0;
	
	assert(size == vertexCount()*3);
}

void SquareTest2D::fillVertex(float* rawData, size_t size) {
	rawData[0] = -.5;
	rawData[1] = -.5;
	rawData[2] = 0.0;
	
	rawData[3] = -.5;
	rawData[4] =  .5;
	rawData[5] = 0.0;
	
	rawData[6]  =  .5;
	rawData[7] = -.5;
	rawData[8] = 0.0;

	rawData[9] =  .5;
	rawData[10] =  .5;
	rawData[11] = 0.0;
	assert(size == vertexCount()*3);
}

}} // ShapeShifter::Opengl

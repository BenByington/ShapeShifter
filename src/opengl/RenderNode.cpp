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
}

size_t RenderNode::vertexCount() const {
	size_t ret = personalCount();
	for (const auto& child : children) {
		ret += child->vertexCount();
	}
	return ret;
}

//TODO unify with TreePopulatColor?  Lots of copy paste...
size_t RenderNode::TreePopulateData(std::vector<float>& vert, std::vector<float>& color, const size_t start) {
	assert(starti_ == 0);

	size_t idx = start;
	for (const auto& child : children) {
		idx += child->TreePopulateData(vert, color, idx);
	}

	starti_ = idx;
	
	fillVertex(vert, idx);
	fillColors(color, idx);
	idx += this->personalCount();

	endi_ = idx;
	return endi_ - starti_;
}

void RenderNode::UpdateData() {

	// TODO find a way to recurse the tree only once maybe?
	size_t size = this->vertexCount();

  std::vector<float> tri_vert(size, 0);
	std::vector<float> tri_col(size, 0);
	std::cerr << tri_vert.size() << std::endl;
	
	size_t end = TreePopulateData(tri_vert, tri_col, 0);
	assert(end == tri_vert.size());

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
		drawRecurse();
    //glDrawArrays (GL_TRIANGLE_STRIP, 0, 12+9);
}

void RenderNode::drawRecurse() const {
	for (const auto& child : children) {
		child->drawRecurse();
	}
	this->drawSelf();
}

//TODO this is ugly don't do this in constructor
SquareTest2D::SquareTest2D() {}
SquareTest2D::~SquareTest2D() {}
size_t SquareTest2D::personalCount() const { return 12; }

void SquareTest2D::fillColors(std::vector<float>& data, size_t start) {
	data[start+0] = 1.0;
	data[start+1] = 0.0;
	data[start+2] = 0.0;
	
	data[start+3] = 1.0;
	data[start+4] = 1.0;
	data[start+5] = 0.0;

	data[start+6] = 0.0;
	data[start+7] = 0.0;
	data[start+8] = 1.0;

	data[start+9] = 0.0;
	data[start+10] = 1.0;
	data[start+11] = 0.0;
}

void SquareTest2D::fillVertex(std::vector<float>& data, size_t start) {
	data[start+0] = -.5;
	data[start+1] = -.5;
	data[start+2] = 0.0;
	
	data[start+3] = -.5;
	data[start+4] =  .5;
	data[start+5] = 0.0;
	
	data[start+6]  =  .5;
	data[start+7] = -.5;
	data[start+8] = 0.0;

	data[start+9] =  .5;
	data[start+10] =  .5;
	data[start+11] = 0.0;
}

TriangleTest2D::TriangleTest2D() {}
TriangleTest2D::~TriangleTest2D() {}
size_t TriangleTest2D::personalCount() const { return 9; }

void TriangleTest2D::fillColors(std::vector<float>& data, size_t start) {
	data[start+0] = 1.0;
	data[start+1] = 0.0;
	data[start+2] = 0.0;
	
	data[start+3] = 1.0;
	data[start+4] = 1.0;
	data[start+5] = 0.0;

	data[start+6] = 0.0;
	data[start+7] = 0.0;
	data[start+8] = 1.0;
}

void TriangleTest2D::fillVertex(std::vector<float>& data, size_t start) {
	data[start+0] = -0.6;
	data[start+1] = -0.6;
	data[start+2] = 0.0;
	
	data[start+3] = -0.6;
	data[start+4] =  0.6;
	data[start+5] = 0.0;
	
	data[start+6]  =  0.6;
	data[start+7] = -0.6;
	data[start+8] = 0.0;
}

void TriangleTest2D::drawSelf() const {
	assert(starti_ % 3 == 0);
	assert(this->vertexCount() % 3 == 0);
  glDrawArrays (GL_LINE_STRIP, starti_/3, this->personalCount()/3);
}

void SquareTest2D::drawSelf() const {
	assert(starti_ % 3 == 0);
	assert(this->vertexCount() % 3 == 0);
	std::cerr << starti_ << " " << personalCount() << std::endl;
  glDrawArrays (GL_TRIANGLE_STRIP, starti_/3, this->personalCount()/3);
}

}} // ShapeShifter::Opengl

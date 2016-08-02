/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RenderNode.h
 * Author: bbyington
 *
 * Created on August 2, 2016, 7:53 AM
 */

#ifndef RENDERNODE_H
#define RENDERNODE_H

// TODO replce with proper non-qt include
#include <QOpenGLWidget>

#include <cstdlib>
#include <memory>
#include <vector>

namespace ShapeShifter {
namespace Opengl {

class RenderNode {
protected:
  RenderNode();
  RenderNode(const RenderNode& orig) = delete;

	//Helpers to keep the tree consistent, while they share a vertex array object.
	virtual size_t vertexCount() const = 0;
	// TODO make safe so we can't overrun buffer?  
	virtual void fillVertex(float* rawData, size_t size) = 0;
	virtual void fillColors(float* rawData, size_t size) = 0;

private:
  void TreePopulateColor(float* rawData, size_t size);
	void TreePopulateVertex(float* rawData, size_t size);
	

public:
  virtual ~RenderNode();
	void AddChild(std::shared_ptr<RenderNode> child);
	void UpdateData();
	void RenderTree() const;

	//TODO move back to private
protected:
  size_t starti_ = 0;
	size_t endi_ = 0;
	size_t size = 0;

	GLuint vao = 0;
	std::vector<std::shared_ptr<RenderNode>> children;
	std::vector<size_t> child_sizes;
};

class SquareTest2D : public RenderNode {
public:
	SquareTest2D();
	virtual ~SquareTest2D();
protected:
	virtual size_t vertexCount() const override;
	virtual void fillVertex(float* rawData, size_t size) override;
	virtual void fillColors(float* rawData, size_t size) override;

 
};

}} // ShapeShifter::Opengl
#endif /* RENDERNODE_H */


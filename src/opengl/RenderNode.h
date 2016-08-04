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
	virtual size_t vertexCount() const;
	// TODO make safe so we can't overrun buffer?  
	virtual void fillVertex(std::vector<float>& rawData, size_t start) = 0;
	virtual void fillColors(std::vector<float>& rawData, size_t start) = 0;

	void drawRecurse() const;
	virtual void drawSelf() const = 0;

	virtual size_t personalCount() const = 0;
private:
  size_t TreePopulateData(std::vector<float>& vert, std::vector<float>& color, size_t start);

public:
  virtual ~RenderNode();
	void AddChild(std::shared_ptr<RenderNode> child);
	void UpdateData();
	void RenderTree() const;

	//TODO move back to private
protected:
	//TODO this is a state machine variable right now...  too brittle, need a new solution
  size_t starti_ = 0;
	size_t endi_ = 0;

	GLuint vao = 0;
	std::vector<std::shared_ptr<RenderNode>> children;
};

class PureNode : public RenderNode {
public:
	PureNode() {}
	virtual ~PureNode() {}
protected:
	virtual size_t personalCount() const override { return 0; }
};

class PointsNode : public RenderNode {
public:
	PointsNode() {}
	virtual ~PointsNode() {}
};

class SquareTest2D : public PointsNode {
public:
	SquareTest2D();
	virtual ~SquareTest2D();
protected:
	virtual size_t personalCount() const override;
	virtual void fillVertex(std::vector<float>& rawData, size_t start) override;
	virtual void fillColors(std::vector<float>& rawData, size_t start) override;
	virtual void drawSelf() const override;
};

class TriangleTest2D : public PointsNode {
public:
	TriangleTest2D();
	virtual ~TriangleTest2D();
protected:
	virtual size_t personalCount() const override;
	virtual void fillVertex(std::vector<float>& rawData, size_t start) override;
	virtual void fillColors(std::vector<float>& rawData, size_t start) override;
	virtual void drawSelf() const override;
};

}} // ShapeShifter::Opengl
#endif /* RENDERNODE_H */


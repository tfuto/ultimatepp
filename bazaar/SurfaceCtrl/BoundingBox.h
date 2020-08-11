#ifndef _SurfaceCtrl_BoundingBox_h_
#define _SurfaceCtrl_BoundingBox_h_

#include "Shader.h"

namespace Upp{

class BoundingBox {
	private:
		bool loaded = false;
		
		glm::vec3 min;
		glm::vec3 max;
		
		Vector<float> BoundingBoxVertices;
		
		GLuint BoundingBoxVAO = 0;
		GLuint BoundingBoxVBO = 0;
		
		void loadBoundingBox(){
			BoundingBoxVertices<<min.x<< min.y<< min.z<<min.x<< min.y<< max.z<<min.x<< max.y<< max.z
			<<max.x<< max.y<< min.z<<min.x<< min.y<< min.z<<min.x<< max.y<< min.z
			<<max.x<< min.y<< max.z<<min.x<< min.y<< min.z<<max.x<< min.y<< min.z
			<<max.x<< max.y<< min.z<<max.x<< min.y<< min.z<<min.x<< min.y<< min.z
			<<min.x<< min.y<< min.z<<min.x<< max.y<< max.z<<min.x<< max.y<< min.z
			<<max.x<< min.y<< max.z<<min.x<< min.y<< max.z<<min.x<< min.y<< min.z
			<<min.x<< max.y<< max.z<<min.x<< min.y<< max.z<<max.x<< min.y<< max.z
			<<max.x<< max.y<< max.z<<max.x<< min.y<< min.z<<max.x<< max.y<< min.z
			<<max.x<< min.y<< min.z<<max.x<< max.y<< max.z<<max.x<< min.y<< max.z
			<<max.x<< max.y<< max.z<<max.x<< max.y<< min.z<<min.x<< max.y<< min.z
			<<max.x<< max.y<< max.z<<min.x<< max.y<< min.z<<min.x<< max.y<< max.z
			<<max.x<< max.y<< max.z<<min.x<< max.y<< max.z<<max.x<< min.y<< max.z;
	
			if(BoundingBoxVAO > 0) glDeleteVertexArrays(1,&BoundingBoxVAO);
			if(BoundingBoxVBO > 0) glDeleteBuffers(1,&BoundingBoxVBO);
			
			glGenVertexArrays(1,&BoundingBoxVAO);
			glGenBuffers(1,&BoundingBoxVBO);
			
			glBindVertexArray(BoundingBoxVAO);
			glBindBuffer(GL_ARRAY_BUFFER,BoundingBoxVBO);
			glBufferData(GL_ARRAY_BUFFER,BoundingBoxVertices.GetCount() * sizeof(float),&(BoundingBoxVertices[0]),GL_STATIC_DRAW);
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
			glEnableVertexAttribArray(0);
			loaded = true;
		}
		
	public:
		BoundingBox(){
			loaded = false;
		}
		BoundingBox(const BoundingBox& copy){
			*this = copy;
		}

		BoundingBox(glm::vec3 min_, glm::vec3 max_){
			min = min_;
			max = max_;
			loadBoundingBox();
		}
		BoundingBox(float minX,float minY, float minZ, float maxX,float maxY,float maxZ){
			min = glm::vec3(minX,minY,minZ);
			max = glm::vec3(maxX,maxY,maxZ);
			loadBoundingBox();
		}
		
		BoundingBox& operator=(const BoundingBox& copy){
			loaded = copy.loaded;
			min = copy.min;
			max = copy.max;
			
			BoundingBoxVertices.Append(copy.BoundingBoxVertices);
			BoundingBoxVAO = copy.BoundingBoxVAO;
			BoundingBoxVBO = copy.BoundingBoxVBO;
			return *this;
		}
		
		BoundingBox& SetBoundingBox(glm::vec3 min_, glm::vec3 max_){
			min = min_;
			max = max_;
			loadBoundingBox();
			return *this;
		}
		BoundingBox& SetBoundingBox(float minX,float minY, float minZ, float maxX,float maxY,float maxZ){
			min = glm::vec3(minX,minY,minZ);
			max = glm::vec3(maxX,maxY,maxZ);
			loadBoundingBox();
			return *this;
		}
		
		bool IsLoaded(){return loaded;}
		
		BoundingBox& TransformBy(const glm::mat4 modelMatrice){
			glm::vec4 min_(min.x,min.y,min.z,0.0f);
			glm::vec4 max_(max.x,max.y,max.z,0.0f);
			min_ = min_ * modelMatrice;
			max_ = max_ * modelMatrice;
			min = glm::vec3(min_.x,min_.y,min_.z);
			max = glm::vec3(max_.x,max_.y,max_.z);
			return *this;
		}
		
		bool LineIntersection(const glm::vec3& start, const glm::vec3& end){
			if(loaded){
				glm::vec3 center     = (min + max) * 0.5f;
			    glm::vec3 extents    = max - center;
			    glm::vec3 lineDir    = 0.5f * (end - start);
			    glm::vec3 lineCenter = start + lineDir;
			    glm::vec3 dir        = lineCenter - center;
			
			    float ld0 = abs(lineDir.x);
			    if (abs(dir.x) > (extents.x + ld0))
			        return false;
			
			    float ld1 = abs(lineDir.y);
			    if (abs(dir.y) > (extents.y + ld1))
			        return false;
			
			    float ld2 = abs(lineDir.z);
			    if (abs(dir.z) > (extents.z + ld2))
			        return false;
			
			    glm::vec3 vCross = glm::cross(lineDir, dir);
			    if (abs(vCross.x) > (extents.y * ld2 + extents.z * ld1))
			        return false;
	
			    if (abs(vCross.y) > (extents.x * ld2 + extents.y * ld0))
			        return false;
			   
			    if (abs(vCross.z) > (extents.x * ld1 + extents.y * ld0))
			        return false;
			
			    return true;
			}
			return false;
		}
		
		BoundingBox& Draw(const glm::mat4& modelMat, const glm::mat4& viewMat, const glm::mat4& projMat, OpenGLProgram& shader){
			if(loaded && shader.IsLinked()){
				shader.Bind();
				shader.SetMat4("ViewMatrix",viewMat);
				shader.SetMat4("ProjectionMatrix",projMat);
				shader.SetMat4("ModelMatrix",modelMat);
				shader.SetVec4("CustomColor", Blue().GetR() / 255.0f, Blue().GetG() / 255.0f, Blue().GetB() / 255.0f, 1.0f );
				glBindVertexArray(BoundingBoxVAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			return *this;
		}
};
}


#endif
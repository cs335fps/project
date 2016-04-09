#ifndef WORLDENGINE_H
#define WORLDENGINE_H
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdio.h>

using namespace std;

class worldEngine
{
	private:
		float norm[3];

		void shift_down(vector<GLfloat>& heap,long i, long max) {
			long i_big, c1, c2;
			while(i < max) {
				i_big = i;
				c1 = (2*i) + 1;
				c2 = c1 + 1;
				if( c1<max && heap[c1]>heap[i_big] )
					i_big = c1;
				if( c2<max && heap[c2]>heap[i_big] )
					i_big = c2;
				if(i_big == i) return;
				swap(heap[i],heap[i_big]);
				i = i_big;
			}
		}

		void to_heap(vector<GLfloat>& arr) {
			long i = (arr.size()/2) - 1;
			for(i; i >= 0; i--) {
				shift_down(arr, i, arr.size());
			}
		}

		void sort(vector<GLfloat>& arr) {
			to_heap(arr);
			int end = arr.size() - 1;
			while (end > 0) {
				swap(arr[0], arr[end]);
				shift_down(arr, 0, end);
				--end;
			}
		}
		int search(GLfloat x)
		{
			int cen = sortvert.size() / 2,
			    end = sortvert.size(), 
			    beg = 0;
			while((end - beg) > 1)
			{
				if(x > cen)
				{
					beg = cen;
					cen += (end-beg) / 2;
				}
				else
				{
					end = cen;
					cen = (end-beg) / 2;
				}
			}
			return cen;
		}
	protected:
		GLfloat x,y,z, rx, ry,rz;
		vector<GLfloat> vertex;				// stores the modle
		vector<GLfloat> sortvert;			// stores the modle
		vector<GLfloat> normals;			// Stores the normals
		vector<GLfloat> Faces_Triangles;		// Stores the triangles
		vector<GLfloat> vertexBuffer;			// Stores the points which make the object
		vector<GLfloat> UVs;			// Stores the points which make the object
		long TotalConnectedPoints;			// Stores the total number of connected verteces
		long TotalConnectedTriangles;			// Stores the total number of connected triangles
		bool solid;

	public:
		worldEngine(string filename)
		{
			string line;
			float X,Y,Z;
			GLfloat u,v,s;
			int tCounter;
			int vertexNumber[3];
			int triangle_index = 0;		// Set triangle index to zero
			int normal_index = 0;		// Set normal index to zero

			solid = true;
			TotalConnectedTriangles = 0;
			TotalConnectedPoints = 0;

			x = y= z =0;
			rx = ry =rz =0;

			ifstream objFile (filename.data());

			// If obj file is open, continue
			if (objFile)
			{
				while(!objFile.eof())
				{
					getline(objFile, line);

					switch(line[0]){
						case 'v':
							line[0] = ' ';
							switch(line[1]){
								case ' ':
									line[1] = ' ';
									// Read floats from the line: v X Y Z
									sscanf(line.data(),"%g %g %g ",&X,&Y,&Z);
									vertex.push_back(X);
									vertex.push_back(Y);
									vertex.push_back(Z);
									TotalConnectedPoints += 3;
									break;
								case 't':
									line[1] = ' ';
									sscanf(line.data(),"%g %g", &u,&v);
									UVs.push_back(u);
									UVs.push_back(v);
									break;
								case 'n':
									line[1] = ' ';
									sscanf(line.data(),"%g %g %g", &u,&v,&s);
									normals.push_back(u);
									normals.push_back(v);
									normals.push_back(s);
									break;
							}
							break;

						case 'f':
							tCounter = 0;
							line[0] = ' ';
							sscanf(line.data(),"%i %i %i",
									&vertexNumber[0],
									&vertexNumber[1],
									&vertexNumber[2]);
							vertexNumber[0] -= 1;
							vertexNumber[1] -= 1;
							vertexNumber[2] -= 1;
							for (int i = 0; i < 3; i++){
								Faces_Triangles.push_back(vertex[(3*vertexNumber[i]) % vertex.size()]);
								Faces_Triangles.push_back(vertex[(3*vertexNumber[i]+1) % vertex.size()]);
								Faces_Triangles.push_back(vertex[(3*vertexNumber[i]+2) % vertex.size()]);
							}

							if(normals.size() == 0){
								float coord1[3] = { 
									Faces_Triangles[triangle_index], 
									Faces_Triangles[triangle_index+1],
									Faces_Triangles[triangle_index+2]};
								float coord2[3] = {
									Faces_Triangles[triangle_index+3],
									Faces_Triangles[triangle_index+4],
									Faces_Triangles[triangle_index+5]};
								float coord3[3] = {
									Faces_Triangles[triangle_index+6],
									Faces_Triangles[triangle_index+7],
									Faces_Triangles[triangle_index+8]};
								calculateNormal( coord1, coord2, coord3 );

								for (int i = 0; i < 3; i++){
									normals.push_back( norm[0]);
									normals.push_back( norm[1]);
									normals.push_back( norm[2]);
								}
							}

							triangle_index += 9;
							TotalConnectedTriangles += 9;
							break;
					}
				}
				objFile.close();

				// coppy verticies and sort
				//sortvert = vertex;
				//sort(sortvert);

			}
		}
		void calculateNormal(float* coord1,float* coord2,float* coord3 )
		{
			float va[3], vb[3], vr[3], val;
			va[0] = coord1[0] - coord2[0];
			va[1] = coord1[1] - coord2[1];
			va[2] = coord1[2] - coord2[2];

			vb[0] = coord1[0] - coord3[0];
			vb[1] = coord1[1] - coord3[1];
			vb[2] = coord1[2] - coord3[2];

			// cross product 
			vr[0] = va[1] * vb[2] - vb[1] * va[2];
			vr[1] = vb[0] * va[2] - va[0] * vb[2];
			vr[2] = va[0] * vb[1] - vb[0] * va[1];

			// normalization factor
			val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );

			norm[0] = vr[0]/val;
			norm[1] = vr[1]/val;
			norm[2] = vr[2]/val;

			return;
		}
		void draw()
		{
			glPushMatrix();

			glEnableClientState(GL_VERTEX_ARRAY);	// Enable vertex arrays
			glEnableClientState(GL_NORMAL_ARRAY);	// Enable normal arrays

			glTranslatef(x,y,z);

			glRotatef(rx, 1.0f,.0f,.0f);		// Rotate On The X axis 
			glRotatef(ry, .0f,1.0f,.0f);		// Rotate On The Y axis 
			glRotatef(rz, .0f,.0f,1.0f);		// Rotate On The Z axis 

			// Vertex Pointer to triangle array
			glVertexPointer(3 ,GL_FLOAT, 0, Faces_Triangles.data());
			// Normal pointer to normal array
			glNormalPointer(GL_FLOAT, 0, normals.data());
			// Draw the triangles
			glDrawArrays(GL_TRIANGLES, 0, TotalConnectedTriangles);
			glDisableClientState(GL_VERTEX_ARRAY);	// Disable vertex arrays
			glDisableClientState(GL_NORMAL_ARRAY);	// Disable normal arrays

			glPopMatrix();
		}
		bool isToutching(float center[], float radius)
		{
			if(solid == true)
				int idx = search(center[0]);
			if(true){
				return true;
			}
			return false;
		}
		void loc(float inX, float inY, float inZ)
		{
			x = inX;
			y = inY;
			z = inZ;
		}
		void rot(float inX, float inY, float inZ)
		{
			rx = inX;
			ry = inY;
			rz = inZ;
		}
		~worldEngine()
		{
			vertex.clear();
			normals.clear();
			Faces_Triangles.clear();
			vertexBuffer.clear();

		}
};
#endif

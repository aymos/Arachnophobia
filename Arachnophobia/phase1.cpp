#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <cstring>
#include <GL/glut.h>
#include <time.h>

using namespace std;

#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)

// Function Declarations
void drawScene();
void update(int value);
void drawBox(float len,float h);
void initRendering();
void handleResize(int w, int h);
void handleKeypress1(unsigned char key, int x, int y);
void handleKeypress2(int key, int x, int y);
void handleMouseclick(int button, int state, int x, int y);
void motion(int x, int y);
void motionPassive(int x, int y);
void SpiderTime(int value);
void drawBall(float rad);
void drawCylinder();
void Highlight();
void drawTriangle();

// Global Variables
int windowWidth;
int windowHeight;
int ispause=0; //false
int score=0,time1=0,l;
int x=0,space=0,s=0;  // s : spider index
float arena_h = 4.13f;
float arena_l = 5.4f;
float cannon1_x=0.0f;
float cannon1_y=-1.8f;
float cannon2_x=0.0f;
float cannon2_y=-1.6f;
float rbucket_x=-1.5f;
float rbucket_y=-1.6f;
float gbucket_x=1.5f;
float gbucket_y=-1.6f;
float laser_x=0.0f;
float laser_y=-1.4f;
float laser_vx=0.0f;
float laser_vy=0.1f;
float l_limit= -2.55f;
float r_limit= 2.55f;
float l_limit1 = -2.55f; //cannon
float r_limit1 = 2.55f;
float l_limit2 = -2.55f; // green
float r_limit2 = 2.55f;
float l_limit3 = -2.55f;  //red
float r_limit3 = 2.55f;
float theta = 0.0f;
float laser_theta = 0.0f; 
bool end = false;
float mouse_x = 0.0f;
float mouse_y = 0.0f;
bool mleft = 0;
bool mright = 0;
float move = 0.15f; // mouse movement sensitivity

//Spiders
int spider[200000]={0};  // 0 means not yet generated. 1 generated. -1 deleted.
float spider_x[200000]={0};
float spider_y[200000]={0};
char spider_color[200000];
float spider_s[200000];

int main(int argc, char **argv) {
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	int w, h;
	if(argc < 6){
		cout<<"\n\n Using default values \n\n";
		l = 1;  //level 
		w = glutGet(GLUT_SCREEN_WIDTH);
		h = glutGet(GLUT_SCREEN_HEIGHT);
	}
	else {
		w = (float)atoi(argv[1]);
		h = (float)atoi(argv[2]);
		l = (int)atoi(argv[3]);
		cannon1_x = (float)atoi(argv[4]);
		cannon2_x = (float)atoi(argv[4]);
		gbucket_x = (float)atoi(argv[5]);
		rbucket_x = (float)atoi(argv[6]);
	}
	windowWidth = w * 2 / 3;
	windowHeight = h * 2 / 3;

	time_t t;
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);

	glutCreateWindow("Arachnophobia");  // Setup the window
	initRendering();

	srand((unsigned) time(&t));

	// Register callbacks
	glutDisplayFunc(drawScene);
	glutIdleFunc(drawScene);
	glutKeyboardFunc(handleKeypress1);
	glutSpecialFunc(handleKeypress2);
	//mouse 
	glutMouseFunc(handleMouseclick);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(motionPassive);

	glutReshapeFunc(handleResize);
	glutTimerFunc(10, update, 0);
	glutTimerFunc(1000,SpiderTime,0);
	glutMainLoop();
	return 0;
}

// Function to draw objects on the screen
void drawScene() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	if ( end == false){
		// Draw Box
		glTranslatef(0.0f, 0.0f, -5.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3f(0.8f, 0.8f, 1.0f);
		drawBox(arena_l,arena_h);

		// Draw Base
		glPushMatrix();
		glTranslatef(0.0f, -(9*arena_h)/10, -5.0f);
		glColor3f(0.9f, 0.7f, 0.2f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		drawBox(2*arena_l,arena_h/5);   
		glPopMatrix();

		// Red Bucket (x=3)
		glPushMatrix();
		glTranslatef(rbucket_x,rbucket_y,-0.5f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if(x == 3)
			glColor3f(1.0f, 0.0f, 0.0f);
		else 
			glColor3f(1.0f, 0.4f, 0.4f);
		drawCylinder();
		if (x == 3)
			glColor3f(1.0f, 1.0f, 1.0f);
		else glColor3f(1.0f, 0.5f, 0.5f);
		Highlight();
		glPopMatrix();

		//Green Bucket (x=1)
		glPushMatrix();
		glTranslatef(gbucket_x,gbucket_y,-0.5f);
		if(x == 1)
			glColor3f(0.0f, 1.0f, 0.0f);
		else glColor3f(0.4f, 1.0f, 0.4f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		drawCylinder();
		if(x == 1)
			glColor3f(1.0f, 1.0f, 1.0f);
		else glColor3f(0.5f, 1.0f, 0.5f);
		Highlight();
		glPopMatrix();

		// Draw Cannon (x=2)
		glPushMatrix();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glTranslatef(cannon1_x,cannon1_y,0.0f);
		glRotatef(theta,0.0f,0.0f,1.0f);
		glColor3f(0.4f, 0.4f, 0.4f);
		if( x == 2 )
			glColor3f(0.0f, 0.0f, 0.0f);
		drawBox(0.45f,0.3f);
		glTranslatef(0,cannon2_y-cannon1_y,0.0f);
		glColor3f(0.4f, 0.4f, 0.4f);
		if( x == 2 )
			glColor3f(0.0f, 0.0f, 0.0f);
		drawBox(0.1f,0.21f);
		glPopMatrix();

		// Laser 
		if(space == 1){
			glPushMatrix();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glTranslatef(laser_x,laser_y,0.0f);
			glRotatef(laser_theta,0.0f,0.0f,1.0f);
			glColor3f(1.0f, 0.0f, 0.0f);
			drawBox(0.03f,0.3f);
			glPopMatrix();
		}

		//Score 
		glPushMatrix();
		char string[1000];
		sprintf(string,"SCORE  %d",score);
		glColor3f(1.0f,0.5f,0.0f);
		glRasterPos3f(+1.6f,1.85f,0.0f);
		for(int i=0;i<strlen(string);i++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15,string[i]);
		glPopMatrix();

		//Spiders 
		int iter=0;
		while(iter<s){
			glPushMatrix();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glTranslatef(spider_x[iter],spider_y[iter],0.0f);
			if(spider_color[iter]=='r')
				glColor3f(1.0f, 0.05f, 0.05f);
			else if(spider_color[iter]=='g')
				glColor3f(0.05f, 1.0f, 0.05f);
			else if(spider_color[iter]=='b')
				glColor3f(0.05f, 0.05f, 0.05f);
			drawBall(0.1f);
			glTranslatef(0.0f,-0.12f,0.0f);
			drawBall(0.05f);
			glTranslatef(0.0f,+0.12f,0.0f);
			glLineWidth(2.0); 
			glBegin(GL_LINES);
			glVertex3f(0.1f,0.0f,0.0f);
			glVertex3f(0.13f,0.03f,0.0f);
			glVertex3f(0.13f,0.03f,0.0f);
			glVertex3f(0.19f,-0.02f,0.0f);

			glVertex3f(-0.1f,0.0f,0.0f);
			glVertex3f(-0.13f,0.03f,0.0f);
			glVertex3f(-0.13f,0.03f,0.0f);
			glVertex3f(-0.19f,-0.02f,0.0f);

			glVertex3f(0.08f,0.06f,0.0f);
			glVertex3f(0.11f,0.11f,0.0f);
			glVertex3f(0.11f,0.11f,0.0f);
			glVertex3f(0.19f,0.04f,0.0f);

			glVertex3f(-0.08f,0.06f,0.0f);
			glVertex3f(-0.11f,0.11f,0.0f);
			glVertex3f(-0.11f,0.11f,0.0f);
			glVertex3f(-0.19f,0.04f,0.0f);

			glVertex3f(0.07f,-0.08f,0.0f);
			glVertex3f(0.10f,-0.05f,0.0f);
			glVertex3f(0.10f,-0.05f,0.0f);
			glVertex3f(0.16f,-0.1f,0.0f);

			glVertex3f(-0.07f,-0.08f,0.0f);
			glVertex3f(-0.10f,-0.05f,0.0f);
			glVertex3f(-0.10f,-0.05f,0.0f);
			glVertex3f(-0.16f,-0.1f,0.0f);
			glEnd();
			if(spider_color[iter]=='b')
				glColor3f(1.0f, 1.0f, 1.0f);
			else 
				glColor3f(0.0f, 0.0f, 0.0f);
			glTranslatef(0.03f,-0.12f,0.0f);
			drawBall(0.015f);
			glTranslatef(-0.06f,0.0f,0.0f);
			drawBall(0.015f);
			glPopMatrix();
			iter++;	
		}
	}

	else if ( end == true ) {
		// Draw Box
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, -5.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3f(0.1f, 0.1f, 0.3f);
		drawBox(arena_l,arena_h);
		glPopMatrix();

		glPushMatrix();
		char string1[1000];
		char string2[100];
		glTranslatef(0.0f, 0.0f, -5.0f);
		sprintf(string1,"GAME OVER  ");
		sprintf(string2," Final Score :  %d",score);
		glColor3f(1.0f, 0.5f, 0.0f);
		glRasterPos3f(-1.8f,0.0f,-5.0f);
		for(int i=0;i<strlen(string1);i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,string1[i]);
		glRasterPos3f(+1.8f,0.0f,+5.0f);

		glColor3f(0.0f, 0.5f, 0.0f);
		glRasterPos3f(-1.8f,-1.0f,-5.0f);
		for(int i=0;i<strlen(string2);i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,string2[i]);

		glPopMatrix();
	}

	//draw mouse arrow

    glPushMatrix();
    glColor3f(0.0f,0.0f,0.0f);
    glTranslatef(mouse_x,mouse_y,0.0f);
    drawTriangle();
    glPopMatrix();


	glPopMatrix();
	glutSwapBuffers();
}

void Highlight(){
	GLUquadricObj *obj = gluNewQuadric();
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluDisk(obj, 0.0, 0.3, 32, 1);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluDeleteQuadric(obj);
}

void drawCylinder(){
	GLUquadricObj *obj = gluNewQuadric();
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluDisk(obj, 0.0, 0.3, 32, 1);
	gluCylinder(obj, 0.3f, 0.3, 0.4, 32, 1);
	glTranslatef(0.0f, 0.0f, 2.0f);
	gluDisk(obj, 0.0, 0.3, 32, 1);
	glTranslatef(0.0f, 0.0f, -2.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluDeleteQuadric(obj);
}


void drawBall(float rad) {
	glBegin(GL_TRIANGLE_FAN);
	for(int i=0 ; i<360 ; i++) {
		glVertex2f(rad * cos(DEG2RAD(i)), rad * sin(DEG2RAD(i)));
	}
	glEnd();
}

//Spider Handling
void SpiderTime(int value){

	if( ispause == 0){	
		spider_x[s]=(float)rand()/(float)(RAND_MAX/5) - 2.5;
		spider_y[s]=1.7f;
		spider[s] = 1;
		int xrandom=rand()%3;
		if(xrandom==0) //red
			spider_color[s]='r';
		if(xrandom==1) //green
			spider_color[s]='g';
		if(xrandom==2) //black
			spider_color[s]='b';
		spider_s[s] = 0.005 + ((float)(rand()%3)) / 400 + 0.0001 * l ;
		++s;
	}
	glutTimerFunc(3000/l,SpiderTime,0);
}

// Function to handle all calculations in the scene
// updated evry 10 milliseconds
void update(int value) {
	if( ispause == 0 && end == false ){
		time1+=10;
		if( laser_x >= 2.55  || laser_x <= - 2.55)
			laser_theta = - laser_theta;

		laser_y += 0.09*cos(DEG2RAD(laser_theta));
		laser_x -= 0.09 * sin(DEG2RAD(laser_theta));
		int i;
		for(i=0;i<s;i++){
			if( spider_y[i]>= -1.75 )
				spider_y[i]-=spider_s[i];   // Speed ranges between 0.01 to 0.03
			else  { 
				if( spider[i] == 1 && ((spider_x[i]-cannon2_x)*(spider_x[i] - cannon2_x)) <= 0.2 * 0.2  && ((cannon2_y - spider_y[i]) * (cannon2_y - spider_y[i] )) <= 0.2 * 0.2 ){
					spider[i] = -2;
					end = true;
					system("aplay OVER.WAV &");
				}
				if(spider[i] == 1) {
					system("aplay NO.WAV &");

					spider[i] = -1;
					score -=5 ;
					if(spider_x[i] > cannon1_x && spider_x[i]<r_limit1 )
						r_limit1=spider_x[i]; 
					else if(spider_x[i] < cannon1_x && spider_x[i]>l_limit1)
						l_limit1=spider_x[i];

					if(spider_x[i] > gbucket_x && spider_x[i]<r_limit2 )
						r_limit2=spider_x[i]; 
					else if(spider_x[i] < gbucket_x && spider_x[i]>l_limit2)
						l_limit2=spider_x[i];

					if(spider_x[i] > rbucket_x && spider_x[i]<r_limit3 )
						r_limit3=spider_x[i]; 
					else if(spider_x[i] < rbucket_x && spider_x[i]>l_limit3)
						l_limit3=spider_x[i];
				}// UH OH >.< Reached Base
			}
		}
		if(time1 >= 1000){
			space=0;	
			time1=0;
		}
		for(i=0;i<s;i++){ 

			if(laser_x <= spider_x[i]+0.25 && laser_x >= spider_x[i]-0.25 && ((laser_y - spider_y[i]) * (laser_y - spider_y[i] )) <= 0.1*0.1 && spider[i] ==1 ){
				spider_x[i]=20.0f;
				spider[i] = -1;
				spider_y[i]=20.0f;
				laser_x=20;
				laser_y=20;
				system("aplay POP.WAV &");
				if(spider_color[i] == 'b')
					score += 1; // shooting black spider fetches 1 point.
			}

			if(((spider_x[i]-rbucket_x)*(spider_x[i] - rbucket_x)) <= 0.3 * 0.3  && ((rbucket_y - spider_y[i]) * (rbucket_y - spider_y[i] )) <= 0.5 * 0.5 && spider[i]==1 ){
				spider_x[i]=20.0f;
				spider_y[i]=20.0f;
				spider[i] = -1;
				if(spider_color[i] == 'r') 
					score += 1;
				else score -=1;
				system("aplay POP.WAV &");
			}
			if(((spider_x[i]-gbucket_x)*(spider_x[i] - gbucket_x)) <= 0.3 * 0.3  && ((gbucket_y - spider_y[i]) * (gbucket_y - spider_y[i] )) <= 0.5*0.5 && spider[i]==1 ){
				spider[i] = -1;
				spider_x[i]=20.0f;
				spider_y[i]=20.0f;
				if(spider_color[i] == 'g') 
					score += 1;
				else score -= 1;
				system("aplay POP.WAV &");
			}
		}
	}
	glutTimerFunc(10, update, 0);
}

void drawBox(float len,float h) {
	glBegin(GL_QUADS);
	glVertex2f(-len / 2, -h / 2);
	glVertex2f(len / 2, -h / 2);
	glVertex2f(len / 2, h / 2);
	glVertex2f(-len / 2, h / 2);
	glEnd();
}

// Initializing some openGL 3D rendering options
void initRendering() {
	glEnable(GL_DEPTH_TEST);        // Enable objects to be drawn ahead/behind one another
	glEnable(GL_COLOR_MATERIAL);    // Enable coloring
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);   // Setting a background color
}

// Function called when the window is resized
void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)w / (float)h, 0.1f, 200.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void handleKeypress1(unsigned char key, int xc, int yc) {
	if(key == 'p')
		ispause=1-ispause;
	if (key == 27 || key == 'q') 
		exit(0);     // escape key is pressed

	if(ispause == 0){
		if (key == 27 || key == 'q') 
			exit(0);     // escape key is pressed
		if(key == 32 && space == 0 ){ //space
			space=1;
			laser_theta=theta;
			laser_x=cannon2_x;
			laser_y=cannon2_y+0.25*sin(DEG2RAD(theta));
			system("aplay LASER1.WAV &");
		}
		if(key == 'g' )  //green bucket
			x=1;
		else if(key == 'b') //cannon
			x=2;	
		else if (key == 'r') //red bucket
			x=3;
		glPopMatrix();
	}
}

void handleKeypress2(int key, int xc, int yc){
	if( ispause == 0){
		if(x==1){
			if (key == GLUT_KEY_LEFT)
				if(gbucket_x<=l_limit2)
					gbucket_x = l_limit2 + 0.15;
				else
					gbucket_x -= 0.06;
			else if (key == GLUT_KEY_RIGHT)
				if(gbucket_x>=r_limit2)
					gbucket_x = r_limit2 - 0.15;
				else
					gbucket_x += 0.06;
		}
		else if(x==2){
			if (key == GLUT_KEY_LEFT){
				if(cannon1_x<=l_limit1){
					cannon1_x = l_limit1 + 0.15;
					cannon2_x = l_limit1 + 0.15;
				}
				else{
					cannon1_x-=0.06;
					cannon2_x-=0.06;
				}
			}
			else if (key == GLUT_KEY_RIGHT){
				if(cannon1_x>=r_limit1){
					cannon1_x = r_limit1 - 0.15;
					cannon2_x = r_limit1 - 0.15;
				}
				else {
					cannon1_x+=0.06;
					cannon2_x+=0.06;
				}
			}
			else if (key == GLUT_KEY_UP){
				if(theta >= -60)
				theta -= 10;
				else theta +=1;
			}
			else if (key == GLUT_KEY_DOWN){
				if(theta <= 60)
				theta += 10;
				else theta -=1;
			}
		}
		else if(x==3){
			if (key == GLUT_KEY_LEFT)
				if(rbucket_x <= l_limit3)
					rbucket_x = l_limit3 + 0.15;
				else
					rbucket_x-=0.06;
			else if (key == GLUT_KEY_RIGHT)
				if(rbucket_x >= r_limit3)
					rbucket_x= r_limit3 - 0.15;
				else
					rbucket_x+=0.06;
		}
	}
}



//mouse clicks 
void handleMouseclick(int button, int state, int xc, int yc) {

	if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mright = 1;
			mleft = 0;
		}
		else
			mright = 0;
	}
	else if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mleft = 1;
			mright = 0;
			if(mouse_x >= gbucket_x - move - 0.3 && mouse_x <= gbucket_x + move + 0.3 && mouse_y <= gbucket_y + 1 && mouse_y >= gbucket_y - 1){
				x=1; 
		}
			// green
			//if(mouse_x >= gbucket_x - move - 0.25f && mouse_x <= gbucket_x + move + 0.25f && mouse_y <= gbucket_y + 0.5f && mouse_y >= gbucket_y - move - 0.25f)
			//	x=1; // green
			else if(mouse_x >= rbucket_x - move - 0.3 && mouse_x <= rbucket_x + move + 0.3 && mouse_y <= rbucket_y + 1 && mouse_y >= rbucket_y - 1){
				x=3 ; //red
			}
			else if(mouse_x >= cannon1_x - move -0.3  && mouse_x <= cannon1_x + move+0.3 && mouse_y <= cannon1_y + 1  && mouse_y >= cannon1_y -1  ){
				x= 2; // cannon
			}
		}
	}

	else
	{
		mleft = 0;
	}

}


void motion(int xc, int yc)
{
	mouse_x = xc;
	mouse_y = yc;
	windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	windowWidth = glutGet(GLUT_WINDOW_WIDTH);

	mouse_x = mouse_x - (windowWidth/2) ;
	mouse_x = (mouse_x*5.4f) / windowWidth;
	mouse_y = (windowHeight/2) - mouse_y ;
	mouse_y = (mouse_y*4.13f) / windowHeight;

	if (mleft)
	{	  
		if( x == 1  ) //green

		{
			if(mouse_x  >= l_limit2 && mouse_x <= r_limit2)
			{
				if(gbucket_x < mouse_x)
					gbucket_x = mouse_x - move;
				else
					gbucket_x = mouse_x + move;
			}
			else
			{
				if(gbucket_x < mouse_x)
					gbucket_x = r_limit2 - move;
				else
					gbucket_x  = l_limit2 + move;
			}


		}
		else if(x==3)  //red
		{
			if(mouse_x >= l_limit3 && mouse_x <= r_limit3)
			{
				if(rbucket_x < mouse_x)
					rbucket_x = mouse_x - move;
				else
					rbucket_x = mouse_x +  move;
			}
			else
			{
				if(rbucket_x < mouse_x)
					rbucket_x = r_limit3 -  move;
				else
					rbucket_x = l_limit3 + move;
			}

		}
		else if(x == 2)
		{ //cannon
			if(mouse_x  >= l_limit1 && mouse_x <= r_limit1)
			{
				if(cannon1_x < mouse_x){
					cannon1_x = mouse_x - move;
					cannon2_x = mouse_x - move;
				}
				else{
					cannon1_x = mouse_x + move;
					cannon2_x = mouse_x + move;
				}
			}
			else
			{
				if(cannon1_x < mouse_x){
					cannon1_x = r_limit1 - move;
					cannon2_x = r_limit1 - move;
				}
				else{
					cannon1_x = l_limit1 + move;
					cannon2_x = l_limit1 + move;
				}
			}
		}
	}

	else if(mright == 1)
	{
		if( x == 2)
		{
			if(cannon1_x < mouse_x){
				if(theta >= -60)
				theta -= 1;
				else theta +=1;
			}
			else{
				if(theta <= 60)
				theta += 1;
				else theta -=1;
			}

		}
	}

}

void motionPassive(int xc, int yc)
{
	mouse_x = xc;
	mouse_y= yc;

	windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	mouse_x = mouse_x - (windowWidth/2)  ;
	mouse_x = (mouse_x*5.4f) / windowWidth;
	mouse_y = (windowHeight/2) - mouse_y ;
	mouse_y = (mouse_y*4.13f) / windowHeight;
}

void drawTriangle() {
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.1f, 0.0f);
    glVertex3f(0.07f, 0.0f, 0.0f);
    glVertex3f(-0.07f, 0.0f, 0.0f);
    glEnd();
}

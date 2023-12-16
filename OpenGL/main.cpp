/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLfloat l = -1.0, r = 1.0, t = 1.0, b = -1.0, zNear = 1, zFar = 6;
GLfloat dr = 5.0 * M_PI / 180;
GLfloat theta[] = { 0,0,0 };
GLfloat alpha[] = { 0,0,0 };
GLfloat beta[] = { 0,0,0 };
GLfloat lamda[] = { 0,0,0,0};

GLfloat z_tank1 = 0, z_tank2 = 0, z_xetai1 = 0, z_xetai2 = 0, z_xetai3 = 0;
GLfloat x_dich1 = 0, x_dich2 = 0, x_dich3 = 0;
GLfloat z_dich1 = 0, z_dich2 = 0, z_dich3 = 0;
GLfloat z_phao = 0;


GLuint model_loc;
mat4 projection;
GLuint projection_loc;
mat4 view;
GLuint view_loc;


GLfloat thetal = 0.0;
GLfloat xEye = 0.0, yEye = 0.0, zEye = 0.0; // tham sô tịnh tiến camera

mat4 model;
mat4 quayBase;
mat4 quayBase2;

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}

/* Khởi tạo các tham số chiếu sáng - tô bóng*/
point4 light_position(0.0, 1.0, 0.0, 1.0); // Vị trí điểm sáng ở phía trên
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0); // Sáng chói

color4 material_ambient(0.2, 0.2, 0.2, 1.0);
color4 material_diffuse(0.8, 0.8, 0.8, 1.0);
color4 material_specular(1.0, 1.0, 1.0, 1.0);
float material_shininess = 50.0;

color4 ambient_product = light_ambient * material_ambient;
color4 diffuse_product = light_diffuse * material_diffuse;
color4 specular_product = light_specular * material_specular;

void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.7,0.7, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}

vec4 maudat = vec4(0.4, 0.2, 0.1, 1.0);
vec4 maubanhxetank = vec4(0, 0, 0, 1.0);
vec4 mauxich = vec4(0.5, 0.5, 0.5, 1.0);
vec4 mauthanxetank = vec4(0.0, 0.5, 0.0, 1.0);
vec4 mauthanxetank2 = vec4(0.0, 0.8, 0.0, 1.0);
vec4 maunongsung = vec4(0.5, 0.5, 0.5, 1.0);
vec4 mauthanphap = vec4(0.5, 0.0, 0.0, 0.8);
vec4 mautrang = vec4(1,1,1,0.5);
vec4 maunongphao = vec4(1.0, 0.5, 0.0, 1.0);
vec4 mauxanhlam = vec4(0.5, 0.5, 0.5, 1.0);
vec4 mautham = vec4(0.0166, 0.0833, 1.0);
vec4 mauvang = vec4(1.0, 0.8, 0.0, 1.0);
vec4 mauoto = vec4(0.0, 0.2, 0.4, 1.0);
vec4 maunaudam = vec4(0.4, 0.2, 0, 1.0);
vec4 vangdam = vec4(1.0, 0.5, 0.0, 1.0);
vec4 den = vec4(0, 0, 0.0, 1.0);
vec4 xanhnuocvien = vec4(0.0, 0.7, 0.7, 1.0);


void matPhang(GLfloat x, GLfloat y, GLfloat z, mat4 mt, vec4 color_of_obj) {

	material_diffuse = color_of_obj;

	diffuse_product = light_diffuse * material_diffuse;

	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	
	point4 eye(0+ xEye, 2 +yEye , 5+ zEye, 1.0);         // vị trí camera ( 0,2,5)
	point4 at(-5* sin(thetal) + xEye, 1 + yEye, -5* cos(thetal) + zEye, 1.0); // điểm nhín tới của camera.

	GLfloat dr = 5.0 * M_PI / 180;
	vec4 up(0, 1, 0, 1.0);

	mat4 v = LookAt(eye, at, up); // camera
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, v);

	mat4 ins = Scale(x, y, z);   // scale
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, quayBase2 *quayBase * mt * ins);

	mat4 p = Frustum(l, r, b, t, zNear, zFar);  // view volumn
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, p);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

// nền
void NenDat() {
	model = Translate(0, 0, 0) ;
	matPhang(8, 0.04, 8, model, maudat);
}

//Xe tăng
void banhxe(GLfloat i) {
	//bánh trc
	model = Translate(-0.4, 0.22, 0) * RotateZ(i);
	matPhang(0.1, 0.4, 0.1, model, maubanhxetank);

	//bánh sau
	model = Translate(0.4, 0.22, 0) * RotateZ(i);
	matPhang(0.1, 0.4, 0.1, model, maubanhxetank);

}
void xichxetank() {
	model = Translate(0, 0.425, 0);
	matPhang(1, 0.01, 0.15, model, mauxich);

	model = Translate(0, 0.025, 0);
	matPhang(1, 0.01, 0.15, model, mauxich);
}
void vongkhung(GLfloat j) {
	model = Translate(0.4, 0.205, 0) *RotateZ(-j)*Translate(0, 0.22,0);
	matPhang(0.015, 0.015, 0.15, model, mauxich);

	model = Translate(-0.4, 0.205, 0) * RotateZ(j) * Translate(0, 0.22, 0);
	matPhang(0.015, 0.015, 0.15, model, mauxich);
}

void cum_banhxe() {
	// làm bánh xe hình tròn bằng việc chạy vòng lặp for cho thanh chữ nhật quay quanh trục 19 lần, mỗi lần quay i*10 độ
	for (int i = 1; i < 20; i++) {
		banhxe(0 + i * 10);
	}
	xichxetank();
	for (int j = 1; j < 44; j++) {
		vongkhung(0 + j * 3.5);
	}

}

void than_tank() {
	model = Translate(0, 0.25, 0.3);
	matPhang(1.1,0.2,0.8, model, mauthanxetank);

	model = Translate(-0.1, 0.5, 0.3) * RotateY(alpha[0]);
	matPhang(0.4, 0.3, 0.4, model, mauthanxetank2);
}
void nong_sung() {
	model = Translate(-0.1, 0.5, 0.3) * RotateY(alpha[0]) * RotateZ(alpha[1]) * Translate(0.5, 0, 0);
	matPhang(1, 0.05, 0.05, model, maunongsung);
}


void the_tank1() {
	quayBase = Translate(-1.8, 0, -1.5 + z_tank1) * Translate(-0.6, 0, -0.3) * RotateY(90 );
	cum_banhxe();
	quayBase = Translate(-1.8, 0, -1.5 + z_tank1) * Translate(0, 0, -0.3) * RotateY(-90 ) ;
	cum_banhxe();

	than_tank();
	nong_sung();
}

void the_tank2() {
	
	quayBase =Translate(-0.8 ,0,-1.5+z_tank2)*Translate(-0.6, 0,-0.3)* RotateY(90) ;
	cum_banhxe();
	quayBase = Translate(-0.8 , 0, -1.5 + z_tank2) * Translate(0, 0, -0.3) * RotateY(-90);
	cum_banhxe();

	than_tank();
	nong_sung();
}

// pháo

void banhxephao(GLfloat i) {
	//bánh 1
	model = Translate(0 + z_phao, 0.22, 0.3) * RotateZ(i);
	matPhang(0.1, 0.4, 0.1, model, maubanhxetank);

	//bánh 2
	model = Translate(0 + z_phao, 0.22, -0.3) * RotateZ(i);
	matPhang(0.1, 0.4, 0.1, model, maubanhxetank);

	//thanh đòn
	model = Translate(z_phao, 0.22,0) ;
	matPhang(0.06, 0.06, 0.8, model, mautrang);

}
void cum_banhxe_phao() {
	// làm bánh xe hình tròn bằng việc chạy vòng lặp for cho thanh chữ nhật quay quanh trục 19 lần, mỗi lần quay i*10 độ
	for (int i = 1; i < 20; i++) {
		banhxephao(0 + i * 10);
	}

}
void thanduoiphap(GLfloat i) {
	model = Translate(0, 0.22, 0)* RotateX(i);
	matPhang(0.8, 0.3, 0.4, model, mauthanphap);
}
void thantruphao() {
	for (int i = 1; i < 44; i++) {
		thanduoiphap( 0 + i * 4);
	}
}
void chanphao() {
	model = Translate(0.3, -0.1, -0.1) *RotateX(30);
	matPhang(0.06, 0.4, 0.04, model, mautham);

	model = Translate(0.3, -0.1, 0.1) * RotateX(-30);
	matPhang(0.06, 0.4, 0.04, model, mautham);
}
void nongphao() {
	model = Translate(0.4, 0.31, 0);
	matPhang(1.3, 0.09, 0.09, model, maunongphao);

	model = Translate(0.95, 0.38, 0);
	matPhang(0.06, 0.06, 0.04, model, maunongphao);
}
void phao() {
	quayBase =Translate(1.5,0,-2 )*  RotateY(theta[0]) * RotateX(theta[1]) * RotateY(-90);
	cum_banhxe_phao();
	quayBase = Translate(1.5, 0, -2) * RotateY(theta[0]) * RotateX(theta[1]) *Translate(0,0.2,z_phao) * RotateY(-90) * RotateZ(20+lamda[1]);
	thantruphao();
	quayBase = Translate(1.5, 0, -2) * RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 0.2, z_phao) * RotateY(-90) * RotateZ(20 + lamda[1])  ;
	chanphao();
	nongphao();
}
void phao2() {
	quayBase = Translate(2.5,0, -2 ) * RotateY(theta[0]) * RotateX(theta[1]) * RotateY(-90);
	cum_banhxe_phao();
	quayBase = Translate(2.5, 0, -2) * RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 0.2, z_phao) * RotateY(-90) * RotateZ(20 + lamda[1]);
	thantruphao();
	quayBase = Translate(2.5, 0, -2) * RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 0.2, z_phao) * RotateY(-90) * RotateZ(20 + lamda[1]);
	chanphao();
	nongphao();
}
void phao3() {
	quayBase = Translate(0.5, 0, -2) * RotateY(theta[0]) * RotateX(theta[1]) * RotateY(-90);
	cum_banhxe_phao();
	quayBase = Translate(0.5, 0, -2) * RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 0.2, z_phao) * RotateY(-90) * RotateZ(20 + beta[4]);
	thantruphao();
	quayBase = Translate(0.5, 0, -2) * RotateY(theta[0]) * RotateX(theta[1]) * Translate(0, 0.2, z_phao) * RotateY(-90) * RotateZ(20 + beta[4]);
	chanphao();
	nongphao();
}
//
void banhxetai(GLfloat i) {
		//bánh phải trên
		model = Translate(0.3, 0.22, 0.4) * RotateX(i) ;
		matPhang(0.1, 0.4, 0.1, model, maubanhxetank);

		//bánh trải dưới
		model = Translate(0.3, 0.22, -0.4)  * RotateX(i);
		matPhang(0.1, 0.4, 0.1, model, maubanhxetank);

		//bánh trái trên
		model = Translate(-0.3, 0.22, 0.4) * RotateX(i);
		matPhang(0.1, 0.4, 0.1, model, maubanhxetank);

		//bánh trái dưới
		model = Translate(-0.3, 0.22, -0.4) * RotateX(i);
		matPhang(0.1, 0.4, 0.1, model, maubanhxetank);

}
void chumbanhxetai() {
	for (int i = 1; i < 20; i++) {
		banhxetai(0 + i * 10);
	}
}

void thanxe() {
	model = Translate(0, 0.35, -0.1) ;
	matPhang(0.8, 0.2, 1, model, mauxanhlam);
}

void thungxe() {
	//đáy thùng
	model = Translate(0, 0.46, -0.1);
	matPhang(0.8, 0.02, 1, model, mauoto);

	// thành1
	model = Translate(0, 0.56, -0.6);
	matPhang(0.8, 0.2, 0.02, model, mauoto);
	// thành2
	model = Translate(0, 0.56, 0.4);
	matPhang(0.8, 0.2, 0.02, model, mauoto);

	// thành3
	model = Translate(0.4, 0.56, -0.1);
	matPhang(0.02 , 0.2, 1, model, mauoto);
	// thành4
	model = Translate(-0.4, 0.56, -0.1);
	matPhang(0.02, 0.2, 1, model, mauoto);

}

void dauxe() {
	model = Translate(0, 0.55, 0.6);
	matPhang(0.8, 0.6, 0.4, model, mauoto);

	//kinh
	model = Translate(0, 0.65, 0.6+ 0.2);
	matPhang(0.6, 0.3, 0.02, model, mautrang);

	model = Translate(0, 0.35, 0.6 + 0.2);
	matPhang(0.5, 0.01, 0.02, model, mautrang);
	model = Translate(0, 0.30, 0.6 + 0.2);
	matPhang(0.5, 0.01, 0.02, model, mautrang);
	model = Translate(0, 0.40, 0.6 + 0.2);
	matPhang(0.5, 0.01, 0.02, model, mautrang);
	// kinh 2 ben
	model = Translate(-0.4, 0.55, 0.6) * Translate(0, 0, 0.2) * RotateY( lamda[0] ) * Translate(0,0,-0.2);
	matPhang(0.02, 0.6, 0.4, model, maunaudam);
	model = Translate(-0.40, 0.65, 0.6 ) * Translate(0, 0, 0.2) * RotateY( lamda[0]) * Translate(0, 0, -0.2);
	matPhang(0.022, 0.15, 0.2, model, mautrang);

	model = Translate(0.4, 0.55, 0.6) * Translate(0, 0, 0.2) * RotateY(-lamda[0]) * Translate(0, 0, -0.2);
	matPhang(0.02, 0.6, 0.4, model, maunaudam);
	model = Translate(0.40, 0.65, 0.6) * Translate(0, 0, 0.2) * RotateY(-lamda[0]) * Translate(0, 0, -0.2);
	matPhang(0.022, 0.15, 0.2, model, mautrang);

	//den`
	model = Translate(0.32, 0.35, 0.8);
	matPhang(0.1, 0.12, 0.02, model, mauvang);

	model = Translate(-0.32, 0.35, 0.8);
	matPhang(0.1, 0.12, 0.02, model, mauvang);
}
void chuyendongthungxe1() {
	quayBase = Translate(2.1, 0.46, -0.5 + z_xetai1)*RotateX(alpha[2])*Translate(0.4,-0.46,0.5);
	thungxe();
}
void chuyendongthungxe2() {
	quayBase = Translate(1.1, 0.46, -0.5 + z_xetai2) * RotateX(alpha[2]) * Translate(0.4, -0.46, 0.5);
	thungxe();
}
void chuyendongthungxe3() {
	quayBase = Translate(0.1, 0.46, -0.5 + z_xetai3) * RotateX(alpha[2]) * Translate(0.4, -0.46, 0.5);
	thungxe();
}
void xetai1(){
	quayBase = Translate(2.5, 0, 0 + z_xetai1);
	chumbanhxetai();
	thanxe();
	dauxe();
	chuyendongthungxe1();
}
void xetai2() {
	quayBase = Translate(1.5, 0, 0 + z_xetai2);
	chumbanhxetai();
	thanxe();
	dauxe();
	chuyendongthungxe2();
}
void xetai3() {
	quayBase = Translate(0.5, 0, 0 + z_xetai3);
	chumbanhxetai();
	thanxe();
	dauxe();
	chuyendongthungxe3();
}

void matcat (GLfloat i ){
	model = Translate(0, 1, 0) * RotateY(i);
	matPhang(0.2, 2, 0.04, model, maunaudam);

	model = RotateY(i)* Translate(0.15, 0.02, 0) * RotateZ(-60) ;
	matPhang(0.24, 0.04, 0.04, model, maunaudam);

	model = RotateY(i) * Translate(0.4, 1.5, 0) ;
	matPhang(0.8, 0.04, 0.04, model, mauthanxetank);
	model = RotateY(i) * Translate(0.3, 2, 0);
	matPhang(0.6, 0.04, 0.04, model, mauthanxetank);
	model = RotateY(i) * Translate(0.2, 2.4, 0);
	matPhang(0.3, 0.04, 0.04, model, mauthanxetank);

	model = RotateY(i) * Translate(0.54, 1.7, 0) * RotateZ(-40);
	matPhang(0.8, 0.04, 0.04, model, mauthanxetank);
	model = RotateY(i) * Translate(0.35, 2.2, 0) * RotateZ(-35);
	matPhang(0.6, 0.04, 0.04, model, mauthanxetank2);
	model = RotateY(i) * Translate(0.22, 2.5, 0) * RotateZ(-35);
	matPhang(0.4, 0.04, 0.04, model, mauthanxetank2);
}

void caicay() {
	for (int i = 1; i < 70; i++) {
		matcat(0 + i*6);
	}
}

void caycoi() {
	quayBase = Translate(-2.8, 0, 0 ) * RotateY(beta[1]) ;
	caicay();
	quayBase = Translate(-2.8, 0, 1) * RotateY(-beta[1]);
	caicay();
	quayBase = Translate(-2.8, 0, 2) * RotateY(beta[1]);
	caicay();
	quayBase = Translate(-2.8, 0, 3) * RotateY(-beta[1]);
	caicay();
}
//song
void khucsong() {

	model = Translate(-2.5, 0.011, 2.5);
	matPhang(3, 0.02, 0.8, model, xanhnuocvien);

	model = Translate(-0, 0.011, 2.2);
	matPhang(3, 0.02, 0.8, model, xanhnuocvien);

	model = Translate(2.5, 0.011, 1.9);
	matPhang(3, 0.02, 0.8, model, xanhnuocvien);
}

void river() {
	quayBase = Translate(0,0,0);
	khucsong();
}
// chòi địch
void matcat_choi(GLfloat i) {
	model = RotateY(0 + i) * Translate(0, 0.6, 0);
	matPhang(1, 1.2, 0.02, model, vangdam);
	model = RotateY(0 + i) * Translate(0.5, 1.2, 0) * RotateZ(-30);
	matPhang(1.2, 0.06, 0.06, model, mauvang);
	model = RotateY(0 + i) * Translate(-0.5, 1.2, 0) * RotateZ(30);
	matPhang(1.2, 0.06, 0.06, model, mauvang);

	model = RotateY(15 + i) * Translate(0, 0.6, 0);
	matPhang(1, 1.2, 0.02, model, vangdam);
	model = RotateY(15 + i) * Translate(0.5, 1.2, 0) * RotateZ(-30);
	matPhang(1.2, 0.06, 0.06, model, mauvang);
	model = RotateY(15 + i) * Translate(-0.5, 1.2, 0) * RotateZ(30);
	matPhang(1.2, 0.06, 0.06, model, mauvang);

	model = RotateY(60 + i) * Translate(0, 0.6, 0);
	matPhang(1, 1.2, 0.02, model, vangdam);
	model = RotateY(60 + i) * Translate(0.5, 1.2, 0) * RotateZ(-30);
	matPhang(1.2, 0.06, 0.06, model, mauvang);
	model = RotateY(60 + i) * Translate(-0.5, 1.2, 0) * RotateZ(30);
	matPhang(1.2, 0.06, 0.06, model, mauvang);

	model = RotateY(-60 + i) * Translate(0, 0.6, 0);
	matPhang(1, 1.2, 0.02, model, vangdam);
	model = RotateY(-60 + i) * Translate(0.5, 1.2, 0) * RotateZ(-30);
	matPhang(1.2, 0.06, 0.06, model, mauvang);
	model = RotateY(-60 + i) * Translate(-0.5, 1.2, 0) * RotateZ(30);
	matPhang(1.2, 0.06, 0.06, model, mauvang);
};

void caichoi() {
	for (int i = 0; i < 44; i++) {
		matcat_choi(0 + i*2);
	}
}

void choidich() {
	quayBase = Translate(3, 0, 3);
	caichoi();
	
}
// dau
void dau(){
	//đầu
	model =  Translate(0, 1, 0)* RotateY(lamda[2]);
	matPhang(0.3, 0.3, 0.3, model, mauvang);

	//mắt
	model = RotateY(lamda[2])*Translate(-0.08, 0, 0) *Translate(-0, 1, 0.16);
	matPhang(0.05, 0.07, 0.02, model, mautrang);
	model = RotateY(lamda[2])*Translate(-0.08, 0, 0)  * Translate(-0, 1.08, 0.16) ;
	matPhang(0.07, 0.01, 0.02, model, den);

	model = RotateY(lamda[2])*Translate(0.08, 1, 0.16) ;
	matPhang(0.05, 0.07, 0.02, model, mautrang);
	model = RotateY(lamda[2])*Translate(0.08, 1.08, 0.16);
	matPhang(0.07, 0.01, 0.02, model, den);
}
void chan() {
	model = Translate(0.1, 0.3, 0)*Translate(0, 0.2, 0) *RotateX(lamda[3])* Translate(0,-0.2,0);
	matPhang(0.14, 0.4, 0.14, model, mauvang);

	model = Translate(-0.1, 0.3, 0)*Translate(0, 0.2, 0)* RotateX(-lamda[3])* Translate(0, -0.2, 0);
	matPhang(0.14, 0.4, 0.14, model, mauvang);

	model = Translate(0.1, 0.05, 0) * Translate(0, 0.45, 0) * RotateX(lamda[3]) * Translate(0, -0.45, 0);
	matPhang(0.14, 0.1, 0.14, model, mautrang);

	model = Translate(-0.1, 0.05, 0) * Translate(0, 0.45, 0) * RotateX(-lamda[3]) * Translate(0, -0.45, 0);
	matPhang(0.14, 0.1, 0.14, model, mautrang);
}
void than() {
	model = Translate(0, 0.6, 0);
	matPhang(0.5, 0.5, 0.3, model, maunongsung);
}
void tay() {

	//khhuy tren
	model = Translate(0.3, 0.7, 0) * RotateZ(30) * Translate(-0.3, 0.1, 0) * RotateX(beta[0]) * Translate(0.3, -0.1, 0);
	matPhang(0.14, 0.3, 0.14, model, vangdam);
	//khhuy duoi
	model = Translate(0.31, 0.55, 0.14) * Translate(-0.6, 0.18, 0) * RotateX(beta[0]) * Translate(0.6, -0.18, 0) * RotateZ(-30) *RotateX(-60);
	matPhang(0.1, 0.2, 0.1, model, vangdam);

	//khhuy tren
	model = Translate(-0.3, 0.7, 0)*RotateZ(-30) * Translate(0.3, 0.1, 0) * RotateX(beta[0]) * Translate(-0.3, -0.1, 0);
	matPhang(0.14, 0.3, 0.14, model, vangdam);
	//khhuy duoi
	model = Translate(-0.31, 0.55, 0.14) * Translate(0.6, 0.18, 0) * RotateX(beta[0]) * Translate(-0.6, -0.18, 0) * RotateZ(30) * RotateX(-60);
	matPhang(0.1, 0.2, 0.1, model, vangdam);
	
}
// địch
void kedich() {
	chan();
	dau();
	than();
	tay();

}
void taphopkedich() {
	quayBase = Translate(1.5 + x_dich1,0,3 +z_dich1) * RotateY(0) ;
	kedich();

	quayBase = Translate(0.6+ x_dich2, 0, 3+z_dich1) * RotateY(-15);
	kedich();

	quayBase = Translate(-0.5+ x_dich3, 0, 3.5+ z_dich1) * RotateY(15);
	kedich();
}

//--------------------------------------------------------
//tổng  hợp 
void TapTran() {
	quayBase = Translate(0, 0, 0);

	NenDat();

	taphopkedich();

	//choidich();
	
	//caycoi();

	//xetai1();

	//xetai2();

	//xetai3();

	//the_tank1();

	//the_tank2();

	/*phao();
	
	phao2();

	phao3();*/

	river();
}
//--------------------------------------------------------
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	TapTran();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
		//điều khiển camera
	case 'd':
		//theta[0] += 5;
		thetal -= 0.1;
		glutPostRedisplay();
		break;
	case 'a':
		//theta[0] -= 5;
		thetal += 0.1;
		glutPostRedisplay();
		break;
	case 'w': // tăng độ xa gần camera
		zEye -= 0.1;
		break;
	case 's': 
		zEye += 0.1;
		break;
	case 'f': // độ cao camera
		yEye += 0.1;
		break;
	case 'F':
		yEye -= 0.1;
		break;
	/*case 'q':
		theta[1] += 5;
		glutPostRedisplay();
		break;
	case 'Q':
		theta[1] -= 5;
		glutPostRedisplay();
		break;*/
	case 'e':
		theta[0] += 5;
		glutPostRedisplay();
		break;
	case 'E':
		theta[0] -= 5;
		glutPostRedisplay();
		break;
	/*case 'r':
		theta[2] += 5;
		glutPostRedisplay();
		break;
	case 'R':
		theta[2] -= 5;
		glutPostRedisplay();
		break;*/
		//tăng  giảm view volumn
	case 'W':
		zNear *= 1.05; zFar *= 1.05;
		
		break;
	case 'S':
		zNear *= 0.95; zFar *= 0.95; 
	
		break;
	case 't': thetal += dr; break;
	case 'T': thetal -= dr; break;

// di chuyển vật thể

	case 'r':
		lamda[1] += 1;
		
		glutPostRedisplay();
		break;
	case 'R':
		lamda[1] -= 1;
		glutPostRedisplay();
		break;
	case '4':
		z_phao += 0.03;
		if (z_phao > 0.5) z_phao = 0.5;
		glutPostRedisplay();
		break;
	case '$':
		z_phao -= 0.03;
		if (z_phao < -1) z_phao = -1;
		glutPostRedisplay();
		break;
	case 'n':
		z_tank1 += 0.15;
		z_tank2 += 0.1;
		if (z_tank1 > 2.25 ) {
			z_tank1 = 2.25;
			z_tank2 = 1.5;
		}
		glutPostRedisplay();
		break;
	case 'N':
		z_tank1 -= 0.15;
		z_tank2 -= 0.1;
		if (z_tank2 < 0 ) {
			z_tank1 = 0;
			z_tank2 = 0;
		}
		
		glutPostRedisplay();
		break;
	case 'h':
		alpha[0] += 5;
		glutPostRedisplay();
		break;
	case 'H':
		alpha[0] -= 5;
		glutPostRedisplay();
		break;
	case 'g':
		alpha[1] += 1;
		if (alpha[1] > 25) alpha[1] = 25;
		glutPostRedisplay();
		break;
	
	case 'G':
		alpha[1] -= 1;
		if (alpha[1] < -5) alpha[1] = -5;
		glutPostRedisplay();
		break;

	case 'm':
		z_xetai1 += 0.05;
		z_xetai2 += 0.08;
		z_xetai3 += 0.11;
		if (z_xetai1 > 0.5 ) {
			z_xetai1 = 0.5;
			z_xetai2 = 0.8;
			z_xetai3 = 1.1;
		}
		glutPostRedisplay();
		break;
	case 'M':
		z_xetai1 -= 0.05;
		z_xetai2 -= 0.08;
		z_xetai3 -= 0.11;
		if (z_xetai1 < 0) {
			z_xetai1 =0;
			z_xetai2 =0;
			z_xetai3 =0;
		}
		glutPostRedisplay();
		break;
	case 'j':
		alpha[2] -= 1;
		if (alpha[2] <-60) alpha[2] = -60;
		glutPostRedisplay();
		break;
	case 'J':
		alpha[2] += 1;
		if (alpha[2] > 0) alpha[2] = 0;
		glutPostRedisplay();
		break;

	case 'y':
		x_dich1 -= 0.1;
		x_dich2 -= 0.17;
		x_dich3 += 0.1;
		
		if (x_dich1 < -1 || x_dich2 < -1.7 || x_dich3 > 1) {
			x_dich1 = -1;
			x_dich2 = -1.7;
			x_dich3 = 1;
		}
		glutPostRedisplay();
		break;
	case 'Y':
		x_dich1 += 0.1;
		x_dich2 += 0.17;
		x_dich3 -= 0.1;
		if (x_dich1 > 0 || x_dich2 > 0 || x_dich3 < 0) {
			x_dich1 = 0;
			x_dich2 = 0;
			x_dich3 = 0;
		}
	case 'i':
		beta[0] += 5;
		if (beta[0] > 20) beta[0] = 20;
		glutPostRedisplay();
		break;
	case 'I':
		beta[0] -= 5;
		if (beta[0] < -20) beta[0] = -20;
		glutPostRedisplay();
		break;
	case '8':
		lamda[2] += 1;
		glutPostRedisplay();
		break;
	case '*':
		lamda[2] -= 1;
		glutPostRedisplay();
		break;
	case '9': 
		z_dich1 += 0.05;
		lamda[3] -= 10;
		glutPostRedisplay();
		break;
	case '(': 
		z_dich1 -= 0.05;
		lamda[3] += 10;
		glutPostRedisplay();
		break;

	case 'o':
		beta[1] += 5;
		glutPostRedisplay();
		break;
	case 'O':
		beta[1] -= 5;
		glutPostRedisplay();
		break;
	case 'k':
		lamda[0] += 5;
		if (lamda[0] > 30) lamda[0] = 30;
		glutPostRedisplay();
		break;
	case 'K':
		lamda[0] -= 5;
		if (lamda[0] < 0) lamda[0] = 0;
		glutPostRedisplay();
		break;
	}
	
	glutPostRedisplay();	
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("TapTran");


	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	
	glutMainLoop();
	return 0;
}

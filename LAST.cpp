#include <iostream>
#include <glut.h>
#include <math.h>
#include <vector>
#include <fstream>
#include <cstring>
#include <map>
#include <ctime>
#include <set>

// код говно
using namespace std;


typedef map<int, char> ic;
typedef set<int, char> is;
typedef vector<pair<int, char>> vic;

// glut 3d vector, not like a real class, but a structure
class Vector {
public:
	float x, y, z, posx, posy, posz;
};


class Side {
private:
	// default colours
	vector<char> col = { 'W', 'G', 'R', 'B', 'O', 'Y' };

public:
	void Change_col(vector<char> col) {
		this->col = col;
	}
	char RightSide(int i) {
		return col[i];
	}
};

class Small_Cube {
private:
	unsigned char _color[3];

public:
	// шесть граней куба - шесть цветов
	unsigned int color[6];
	// размер ребра
	double size;

	void rotateRR() {
		unsigned int tmp = color[5];
		color[5] = color[3];
		color[3] = color[4];
		color[4] = color[2];
		color[2] = tmp;
	}

	void rotateGR() {
		unsigned int tmp = color[2];
		color[2] = color[1];
		color[1] = color[3];
		color[3] = color[0];
		color[0] = tmp;
	}

	void rotateGD() {
		unsigned int tmp = color[0];
		color[0] = color[4];
		color[4] = color[1];
		color[1] = color[5];
		color[5] = tmp;
	}

	void setColor(int i, int color) {
		this->color[i] = color;
	}

	unsigned char* at(int i) {
		// разбиваем color[i] на 3 составляющих
		// для 0xFF0000 RGB(FF, 0, 00) - красный цвет;
		_color[0] = color[i] >> 16;
		_color[1] = color[i] >> 8;
		_color[2] = color[i];
		return _color;
	}

	// отрисовка куба:
	// устанавливаем цвет и нормали
	void draw() {
		glPushMatrix();
		glBegin(GL_QUADS);

		// верх
		glColor3ubv(at(0));
		glNormal3f(0, 0, 1);
		glVertex3f(size, size, size);
		glVertex3f(0, size, size);
		glVertex3f(0, 0, size);
		glVertex3f(size, 0, size);

		// низ
		glColor3ubv(at(1));
		glNormal3f(0, 0, -1);
		glVertex3f(size, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, size, 0);
		glVertex3f(size, size, 0);

		// спереди
		glColor3ubv(at(2));
		glNormal3f(0, -1, 0);
		glVertex3f(size, 0, size);
		glVertex3f(0, 0, size);
		glVertex3f(0, 0, 0);
		glVertex3f(size, 0, 0);

		// сзади
		glColor3ubv(at(3));
		glNormal3f(0, 1, 0);
		glVertex3f(size, size, 0);
		glVertex3f(0, size, 0);
		glVertex3f(0, size, size);
		glVertex3f(size, size, size);

		// слева
		glColor3ubv(at(4));
		glNormal3f(-1, 0, 0);
		glVertex3f(0, size, size);
		glVertex3f(0, size, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, size);

		// справа
		glColor3ubv(at(5));
		glNormal3f(1, 0, 0);
		glVertex3f(size, size, 0);
		glVertex3f(size, size, size);
		glVertex3f(size, 0, size);
		glVertex3f(size, 0, 0);

		glEnd();
		glPopMatrix();
	}

	// отрисовка куба со смещением (x, y, z)
	void draw(double x, double y, double z) {
		glPushMatrix();
		glTranslated(x, y, z);
		draw();
		glPopMatrix();
	}

};


class Ccube :public Side {
private:
	int k;
	int swaps;
	vector<vector<char>> cube;
	// для отрисовки
	int _angle[3];
	bool ok[3][3][3];
	Small_Cube tmp[3][3];
	// 27 parts
	Small_Cube a[3][3][3];
	// храним угол поворота каждой грани
	int rotate[6];
	// размер кубика-рубика
	double size;
	// цвета граней
	unsigned int color[6];
	// скорость поворота, должно быть 90 % speed_torn = 0 
	int speed_torn = 12;


	// 
	// 		      5 6 7
	// 		      4 8 0
	// 		      3 2 1

	//    6 7 0   6 7 0   6 7 0   6 7 0
	//    5 8 1   6 8 1   5 8 1   5 8 1
	//    4 3 2   4 3 2   4 3 2   4 3 2

	// 		      3 2 1
	// 		      4 8 0
	// 		      5 6 7
	// 

	void clockRot(char col) {

		switch (col)
		{
		case 'W':

			// rotation of the face
			// right
			swap(cube[0][7], cube[0][3]);
			swap(cube[0][6], cube[0][4]);
			swap(cube[0][0], cube[0][2]);
			swap(cube[0][7], cube[0][5]);
			swap(cube[0][0], cube[0][4]);
			swap(cube[0][1], cube[0][3]);

			// rot adj

			swap(cube[3][0], cube[4][0]);
			swap(cube[3][7], cube[4][7]);
			swap(cube[3][6], cube[4][6]);
			swap(cube[4][6], cube[1][6]);
			swap(cube[4][7], cube[1][7]);
			swap(cube[4][0], cube[1][0]);
			swap(cube[1][6], cube[2][6]);
			swap(cube[1][7], cube[2][7]);
			swap(cube[1][0], cube[2][0]);

			Rotate(3, speed_torn, -1);

			break;

		case 'R':

			swap(cube[2][0], cube[2][4]);
			swap(cube[2][7], cube[2][5]);
			swap(cube[2][1], cube[2][3]);
			swap(cube[2][0], cube[2][6]);
			swap(cube[2][1], cube[2][5]);
			swap(cube[2][2], cube[2][4]);

			swap(cube[3][6], cube[0][3]);
			swap(cube[3][5], cube[0][2]);
			swap(cube[3][4], cube[0][1]);
			swap(cube[0][1], cube[1][0]);
			swap(cube[0][2], cube[1][1]);
			swap(cube[0][3], cube[1][2]);
			swap(cube[1][0], cube[5][3]);
			swap(cube[1][1], cube[5][2]);
			swap(cube[1][2], cube[5][1]);

			Rotate(5, speed_torn, -1);

			break;


		case 'Y':

			swap(cube[5][1], cube[5][5]);
			swap(cube[5][2], cube[5][4]);
			swap(cube[5][0], cube[5][6]);
			swap(cube[5][1], cube[5][3]);
			swap(cube[5][0], cube[5][4]);
			swap(cube[5][7], cube[5][5]);

			swap(cube[3][4], cube[2][4]);
			swap(cube[3][3], cube[2][3]);
			swap(cube[3][2], cube[2][2]);
			swap(cube[2][2], cube[1][2]);
			swap(cube[2][3], cube[1][3]);
			swap(cube[2][4], cube[1][4]);
			swap(cube[1][4], cube[4][4]);
			swap(cube[1][3], cube[4][3]);
			swap(cube[1][2], cube[4][2]);

			Rotate(2, speed_torn, 1);

			break;

		case 'O':

			swap(cube[4][4], cube[4][0]);
			swap(cube[4][3], cube[4][1]);
			swap(cube[4][5], cube[4][7]);
			swap(cube[4][4], cube[4][2]);
			swap(cube[4][5], cube[4][1]);
			swap(cube[4][6], cube[4][0]);

			swap(cube[3][2], cube[5][5]);
			swap(cube[3][1], cube[5][6]);
			swap(cube[3][0], cube[5][7]);
			swap(cube[5][5], cube[1][6]);
			swap(cube[5][6], cube[1][5]);
			swap(cube[5][7], cube[1][4]);
			swap(cube[1][6], cube[0][7]);
			swap(cube[1][5], cube[0][6]);
			swap(cube[1][4], cube[0][5]);

			Rotate(4, speed_torn, 1);

			break;

		case 'G':

			swap(cube[1][6], cube[1][2]);
			swap(cube[1][5], cube[1][3]);
			swap(cube[1][7], cube[1][1]);
			swap(cube[1][4], cube[1][6]);
			swap(cube[1][7], cube[1][3]);
			swap(cube[1][0], cube[1][2]);

			swap(cube[0][5], cube[4][2]);
			swap(cube[0][4], cube[4][1]);
			swap(cube[0][3], cube[4][0]);
			swap(cube[5][3], cube[4][2]);
			swap(cube[5][4], cube[4][1]);
			swap(cube[5][5], cube[4][0]);
			swap(cube[5][3], cube[2][6]);
			swap(cube[5][4], cube[2][5]);
			swap(cube[5][5], cube[2][4]);

			Rotate(1, speed_torn, -1);

			break;

		case 'B':

			swap(cube[3][1], cube[3][7]);
			swap(cube[3][2], cube[3][6]);
			swap(cube[3][5], cube[3][3]);
			swap(cube[3][2], cube[3][0]);
			swap(cube[3][7], cube[3][3]);
			swap(cube[3][6], cube[3][4]);

			swap(cube[5][1], cube[4][4]);
			swap(cube[5][0], cube[4][5]);
			swap(cube[5][7], cube[4][6]);
			swap(cube[0][7], cube[4][4]);
			swap(cube[0][0], cube[4][5]);
			swap(cube[0][1], cube[4][6]);
			swap(cube[0][7], cube[2][0]);
			swap(cube[0][0], cube[2][1]);
			swap(cube[0][1], cube[2][2]);

			Rotate(0, speed_torn, 1);

			break;
		}

		k++;

	}

	void aclockRot(char col) {

		clockRot(col);
		clockRot(col);
		clockRot(col);
		k = k - 2;

	}

	// im about to loose my life

	bool checkCube() {
		// check centers
		if (cube[0][8] != 'W' || cube[1][8] != 'G' || cube[2][8] != 'R' || cube[3][8] != 'B' || cube[4][8] != 'O' || cube[5][8] != 'Y') {
			return false;
		}
		// count the number of col
		int w, g, r, b, o, y;
		w = g = r = b = o = y = 0;
		char c;


		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 9; j++) {

				c = cube[i][j];

				switch (c)
				{

				case 'W':
					w++;
					break;

				case 'G':
					g++;
					break;

				case 'R':
					r++;
					break;

				case 'B':
					b++;
					break;

				case 'O':
					o++;
					break;

				case 'Y':
					y++;
					break;
				}
			}
		}

		if (!(w != g != r != b != o != y != 9)) {
			return false;
		}

		return true;
	}

	vector<vector<char>> gaga() {
		return cube;
	}


public:

	void Resize_cube() {

		this->cube.resize(6, vector<char>(9));

		int i = 0;
		k = 0;
		swaps = 0;

		while (i != 6) {

			for (int j = 0; j < 9; ++j) {
				cube[i][j] = RightSide(i);
			}

			++i;
		}


	}

	void Show_cube() {
		cout << "- - - - - - - - - - - - - - - - - - - - - - - -";
		cout << '\n';
		cout << '\n';
		cout << "          ";
		cout << cube[0][5] << ' ' << cube[0][6] << ' ' << cube[0][7] << ' ' << endl;
		cout << "          ";
		cout << cube[0][4] << ' ' << cube[0][8] << ' ' << cube[0][0] << ' ' << endl;
		cout << "          ";
		cout << cube[0][3] << ' ' << cube[0][2] << ' ' << cube[0][1] << ' ' << endl;
		cout << '\n';

		cout << cube[1][6] << ' ' << cube[1][7] << ' ' << cube[1][0] << ' ' << "    ";
		cout << cube[2][6] << ' ' << cube[2][7] << ' ' << cube[2][0] << ' ' << "    ";
		cout << cube[3][6] << ' ' << cube[3][7] << ' ' << cube[3][0] << ' ' << "    ";
		cout << cube[4][6] << ' ' << cube[4][7] << ' ' << cube[4][0] << ' ' << "    ";
		cout << endl;


		cout << cube[1][5] << ' ' << cube[1][8] << ' ' << cube[1][1] << ' ' << "    ";
		cout << cube[2][5] << ' ' << cube[2][8] << ' ' << cube[2][1] << ' ' << "    ";
		cout << cube[3][5] << ' ' << cube[3][8] << ' ' << cube[3][1] << ' ' << "    ";
		cout << cube[4][5] << ' ' << cube[4][8] << ' ' << cube[4][1] << ' ' << "    ";
		cout << endl;

		cout << cube[1][4] << ' ' << cube[1][3] << ' ' << cube[1][2] << ' ' << "    ";
		cout << cube[2][4] << ' ' << cube[2][3] << ' ' << cube[2][2] << ' ' << "    ";
		cout << cube[3][4] << ' ' << cube[3][3] << ' ' << cube[3][2] << ' ' << "    ";
		cout << cube[4][4] << ' ' << cube[4][3] << ' ' << cube[4][2] << ' ' << "    ";

		cout << endl;
		cout << endl;

		cout << "          ";
		cout << cube[5][5] << ' ' << cube[5][6] << ' ' << cube[5][7] << ' ' << endl;
		cout << "          ";
		cout << cube[5][4] << ' ' << cube[5][8] << ' ' << cube[5][0] << ' ' << endl;
		cout << "          ";
		cout << cube[5][3] << ' ' << cube[5][2] << ' ' << cube[5][1] << ' ' << endl;
		cout << '\n';
		cout << "- - - - - - - - - - - - - - - - - - - - - - - -";
		cout << '\n';

	}


	void Save() {

		ofstream fout("saved.txt");

		fout << "- - - - - - - - - - - - - - - - - - - - - - - -";
		fout << '\n';
		fout << '\n';
		fout << "          ";
		fout << cube[0][5] << ' ' << cube[0][6] << ' ' << cube[0][7] << ' ' << endl;
		fout << "          ";
		fout << cube[0][4] << ' ' << cube[0][8] << ' ' << cube[0][0] << ' ' << endl;
		fout << "          ";
		fout << cube[0][3] << ' ' << cube[0][2] << ' ' << cube[0][1] << ' ' << endl;
		fout << '\n';

		fout << cube[1][6] << ' ' << cube[1][7] << ' ' << cube[1][0] << ' ' << "    ";
		fout << cube[2][6] << ' ' << cube[2][7] << ' ' << cube[2][0] << ' ' << "    ";
		fout << cube[3][6] << ' ' << cube[3][7] << ' ' << cube[3][0] << ' ' << "    ";
		fout << cube[4][6] << ' ' << cube[4][7] << ' ' << cube[4][0] << ' ' << "    ";
		fout << endl;


		fout << cube[1][5] << ' ' << cube[1][8] << ' ' << cube[1][1] << ' ' << "    ";
		fout << cube[2][5] << ' ' << cube[2][8] << ' ' << cube[2][1] << ' ' << "    ";
		fout << cube[3][5] << ' ' << cube[3][8] << ' ' << cube[3][1] << ' ' << "    ";
		fout << cube[4][5] << ' ' << cube[4][8] << ' ' << cube[4][1] << ' ' << "    ";
		fout << endl;

		fout << cube[1][4] << ' ' << cube[1][3] << ' ' << cube[1][2] << ' ' << "    ";
		fout << cube[2][4] << ' ' << cube[2][3] << ' ' << cube[2][2] << ' ' << "    ";
		fout << cube[3][4] << ' ' << cube[3][3] << ' ' << cube[3][2] << ' ' << "    ";
		fout << cube[4][4] << ' ' << cube[4][3] << ' ' << cube[4][2] << ' ' << "    ";

		fout << endl;
		fout << endl;

		fout << "          ";
		fout << cube[5][5] << ' ' << cube[5][6] << ' ' << cube[5][7] << ' ' << endl;
		fout << "          ";
		fout << cube[5][4] << ' ' << cube[5][8] << ' ' << cube[5][0] << ' ' << endl;
		fout << "          ";
		fout << cube[5][3] << ' ' << cube[5][2] << ' ' << cube[5][1] << ' ' << endl;
		fout << '\n';
		fout << "- - - - - - - - - - - - - - - - - - - - - - - -";
		fout << '\n';

		cout << "cube saved" << endl;

		fout.close();

	}

	void Load(string FILE_NAME) {

		char c;
		ifstream fin(FILE_NAME);
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 9; j++) {

				fin >> c;
				cube[i][j] = c;

			}
		}

		if (checkCube())
		{
			cout << "Kubik is ok" << endl;
		}
		else {
			cout << "SLAVE" << '\n';
		}

	}

	void rot(char i) {
		clockRot(i);
	}

	void White_bottom(char q) {

		if ((cube[5][0] == 'W' && cube[3][3] == q) || (cube[5][2] == 'W' && cube[2][3] == q) || (cube[5][4] == 'W' && cube[1][3] == q) || (cube[5][6] == 'W' && cube[4][3] == q)) {

			switch (q)
			{

			case 'B':

				while (cube[3][3] != q || cube[5][0] != 'W')
				{
					clockRot('Y');
				}
				break;


			case 'R':

				while (cube[2][3] != q || cube[5][2] != 'W')
				{
					clockRot('Y');
				}
				if (q != 'B')
				{
					while (cube[0][0] != 'W' && cube[3][7] != 'B')
					{
						clockRot('W');
					}
				}
				break;


			case 'G':

				while (cube[1][3] != q || cube[5][4] != 'W')
				{
					clockRot('Y');
				}
				if (q != 'B')
				{
					while (cube[0][0] != 'W' && cube[3][7] != 'B')
					{
						clockRot('W');
					}
				}

				break;


			case 'O':

				while (cube[4][3] != q || cube[5][6] != 'W')
				{
					clockRot('Y');
				}
				if (q != 'B')
				{
					while (cube[0][0] != 'W' && cube[3][7] != 'B')
					{
						clockRot('W');
					}
				}

				break;

			}

			clockRot(q);
			clockRot(q);

		}
	}

	void Right_alg(char a, char c) {

		aclockRot(a);
		clockRot('Y');
		clockRot(a);
		White_bottom(c);
	}

	void White_right(char q) {

		if (cube[3][1] == 'W' || cube[2][1] == 'W' || cube[1][1] == 'W' || cube[4][1] == 'W') {
			if (cube[3][5] == q && cube[2][1] == 'W') {
				Right_alg('B', q);
			}
			if (cube[2][5] == q && cube[1][1] == 'W') {
				Right_alg('R', q);
			}
			if (cube[1][5] == q && cube[4][1] == 'W') {
				Right_alg('G', q);
			}
			if (cube[4][5] == q && cube[3][1] == 'W') {
				Right_alg('O', q);
			}
		}

	}

	void Left_alg(char a, char c) {
		clockRot(a);
		clockRot('Y');
		aclockRot(a);
		White_bottom(c);
	}

	void White_left(char q) {

		if (cube[3][5] == 'W' || cube[2][5] == 'W' || cube[1][5] == 'W' || cube[4][5] == 'W') {

			if (cube[3][5] == 'W' && cube[2][1] == q) {

				Left_alg('R', q);

			}
			if (cube[2][5] == 'W' && cube[1][1] == q) {

				Left_alg('G', q);

			}
			if (cube[1][5] == 'W' && cube[4][1] == q) {

				Left_alg('O', q);

			}
			if (cube[4][5] == 'W' && cube[3][1] == q) {

				Left_alg('B', q);

			}
		}
	}

	void Top_alg(char a, char B, char c) {

		aclockRot(a);
		clockRot('W');
		clockRot(B);
		aclockRot('W');
		White_bottom(c);

	}

	void White_Top(char q) {

		if (cube[3][7] == 'W' && cube[0][0] == q) {

			Top_alg('B', 'R', q);

		}
		if (cube[2][7] == 'W' && cube[0][2] == q) {

			Top_alg('R', 'G', q);

		}
		if (cube[1][7] == 'W' && cube[0][4] == q) {

			Top_alg('G', 'O', q);

		}
		if (cube[4][7] == 'W' && cube[0][6] == q) {

			Top_alg('O', 'B', q);

		}
	}

	void Inv_alg(char a, char B, char c) {

		clockRot(a);
		clockRot(B);
		aclockRot('Y');
		aclockRot(B);
		aclockRot(a);
		White_bottom(c);

	}

	void White_bottom_inverted(char q) {

		if (cube[3][3] == 'W' || cube[2][3] == 'W' || cube[1][3] == 'W' || cube[4][3] == 'W') {

			if (cube[3][3] == 'W' && cube[5][0] == q) {

				Inv_alg('B', 'R', q);

			}
			if (cube[2][3] == 'W' && cube[5][2] == q) {

				Inv_alg('R', 'G', q);

			}
			if (cube[1][3] == 'W' && cube[5][4] == q) {

				Inv_alg('G', 'O', q);

			}
			if (cube[4][3] == 'W' && cube[5][6] == q) {

				Inv_alg('O', 'B', q);

			}
		}

	}

	void Solve_White_cross() {

		char adj[4] = { 'B','R','G','O' };

		for (int i = 0; i < 4; i++) {

			if (cube[0][0] == 'W' && cube[3][7] == adj[i]) {
				clockRot('B');
			}

			if (cube[0][2] == 'W' && cube[2][7] == adj[i]) {
				clockRot('R');
			}

			if (cube[0][4] == 'W' && cube[1][7] == adj[i]) {
				clockRot('G');
			}

			if (cube[0][6] == 'W' && cube[4][7] == adj[i]) {
				clockRot('O');
			}

			White_bottom(adj[i]);
			White_bottom_inverted(adj[i]);
			White_left(adj[i]);
			White_right(adj[i]);
			White_Top(adj[i]);

			if (i != 0) {
				while (cube[3][7] != 'B') { clockRot('W'); }
			}

			if (cube[0][0] == 'W' && cube[0][2] == 'W' && cube[0][4] == 'W' && cube[0][6] == 'W' && cube[3][7] == 'B' && cube[2][7] == 'R' && cube[1][7] == 'G' && cube[4][7] == 'O') {

				break;

			}

		}

	}

	void White_corners_alg_left() {

		aclockRot('B');

		aclockRot('Y');

		clockRot('B');

	}
	void White_corners_alg_right() {

		clockRot('R');
		clockRot('Y');
		aclockRot('R');

	}

	void Solve_white_corners() {

		while (cube[2][0] != 'R' || cube[2][6] != 'R' || cube[3][0] != 'B' || cube[3][6] != 'B' || cube[4][0] != 'O' || cube[4][6] != 'O' || cube[1][0] != 'G' || cube[1][6] != 'G') {

			while (cube[2][7] != 'R') {

				clockRot('W');

			}

			if (cube[3][4] == 'W' || cube[2][4] == 'W' || cube[1][4] == 'W' || cube[4][4] == 'W') {

				while (cube[3][4] != 'W') {

					clockRot('Y');

				}

				while (cube[2][2] != cube[2][7]) {

					clockRot('W');

				}

				White_corners_alg_left();

				while (cube[2][7] != 'R') {

					clockRot('W');

				}

			}

			else if (cube[3][2] == 'W' || cube[2][2] == 'W' || cube[1][2] == 'W' || cube[4][2] == 'W') {

				while (cube[2][2] != 'W') {

					clockRot('Y');

				}

				while (cube[2][7] != cube[5][1]) {

					clockRot('W');

				}

				White_corners_alg_right();

				while (cube[2][7] != 'R') {

					clockRot('W');

				}

			}

			else if (cube[5][1] == 'W' || cube[5][3] == 'W' || cube[5][5] == 'W' || cube[5][7] == 'W') {

				while (cube[5][1] != 'W') {

					clockRot('Y');

				}

				while (cube[2][2] != cube[3][7]) {

					clockRot('W');

				}

				aclockRot('B');
				clockRot('Y');
				clockRot('Y');
				clockRot('B');

				while (cube[3][4] != 'W') {

					clockRot('Y');

				}

				while (cube[2][2] != cube[2][7]) {

					clockRot('W');

				}

				White_corners_alg_left();

				while (cube[2][7] != 'R') {

					clockRot('W');

				}

			}

			else {

				while (cube[2][7] == cube[2][0]) {

					clockRot('W');

				}

				White_corners_alg_left();

				while (cube[2][7] != 'R') {

					clockRot('W');

				}

			}
		}
	}

	void middle_place_Left_alg(char left, char centeR) {

		aclockRot('Y');
		aclockRot(left);
		clockRot('Y');
		clockRot(left);
		clockRot('Y');
		clockRot(centeR);
		aclockRot('Y');
		aclockRot(centeR);

	}

	void middle_place_Right_alg(char centeR, char RiGht) {

		clockRot('Y');
		clockRot(RiGht);
		aclockRot('Y');
		aclockRot(RiGht);
		aclockRot('Y');
		aclockRot(centeR);
		clockRot('Y');
		clockRot(centeR);

	}

	void Solve_middle_layer() {

		while (cube[2][5] != 'R' || cube[2][1] != 'R' || cube[3][1] != 'B' || cube[3][5] != 'B' || cube[4][1] != 'O' || cube[4][5] != 'O' || cube[1][1] != 'G' || cube[1][5] != 'G') {

			if ((cube[4][1] != 'O' && cube[1][5] != 'G') && (cube[4][1] != 'Y' || cube[1][5] != 'Y')) {

				while (cube[1][3] != 'Y' && cube[5][4] != 'Y') {

					clockRot('Y');
				}

				middle_place_Right_alg('G', 'O');

			}

			else if ((cube[4][5] != 'O' && cube[3][1] != 'B') && (cube[4][5] != 'Y' || cube[3][1] != 'Y')) {

				while (cube[4][3] != 'Y' && cube[5][6] != 'Y') {

					clockRot('Y');

				}

				middle_place_Right_alg('O', 'B');

			}

			else if ((cube[3][5] != 'B' && cube[2][1] != 'R') && (cube[3][5] != 'Y' || cube[2][1] != 'Y')) {

				while (cube[3][3] != 'Y' && cube[5][0] != 'Y') {

					clockRot('Y');

				}

				middle_place_Right_alg('B', 'R');

			}

			else if ((cube[2][5] != 'R' && cube[1][1] != 'G') && (cube[2][5] != 'Y' || cube[1][1] != 'Y')) {

				while (cube[2][3] != 'Y' && cube[5][2] != 'Y') {

					clockRot('Y');

				}

				middle_place_Right_alg('R', 'G');

			}

			while (cube[2][3] == 'Y' || cube[5][2] == 'Y') {

				clockRot('Y');

			}

			if (cube[2][3] == 'R' && cube[5][2] != 'Y') {

				if (cube[5][2] == 'G') {

					middle_place_Right_alg('R', 'G');

				}

				else if (cube[5][2] == 'B') {

					middle_place_Left_alg('B', 'R');

				}

			}

			else if (cube[2][3] == 'B' && cube[5][2] != 'Y') {

				clockRot('Y');

				if (cube[5][0] == 'R') {

					middle_place_Right_alg('B', 'R');

				}

				else if (cube[5][0] == 'O') {

					middle_place_Left_alg('O', 'B');

				}
			}

			else if (cube[2][3] == 'O' && cube[5][2] != 'Y') {

				clockRot('Y');
				clockRot('Y');

				if (cube[5][6] == 'B') {

					middle_place_Right_alg('O', 'B');

				}
				else if (cube[5][6] == 'G') {

					middle_place_Left_alg('G', 'O');

				}
			}

			else if (cube[2][3] == 'G' && cube[5][2] != 'Y') {

				aclockRot('Y');

				if (cube[5][4] == 'O') {

					middle_place_Right_alg('G', 'O');

				}

				else if (cube[5][4] == 'R') {

					middle_place_Left_alg('R', 'G');

				}
			}
		}
	}

	void Yellow_cross_alg() {

		clockRot('R');
		clockRot('Y');
		clockRot('G');
		aclockRot('Y');
		aclockRot('G');
		aclockRot('R');

	}

	void Yellow_cross() {

		while (cube[5][0] != 'Y' || cube[5][2] != 'Y' || cube[5][4] != 'Y' || cube[5][6] != 'Y') {

			if ((cube[5][0] == 'Y' && cube[5][6] == 'Y') || (cube[5][4] == 'Y' && cube[5][6] == 'Y') || (cube[5][2] == 'Y' && cube[5][4] == 'Y') || (cube[5][0] == 'Y' && cube[5][2] == 'Y')) {

				while (cube[5][0] != 'Y' && cube[5][6] != 'Y') {

					clockRot('Y');

				}

				Yellow_cross_alg();

			}

			if ((cube[5][2] == 'Y' && cube[5][6] == 'Y') || (cube[5][0] == 'Y' && cube[5][4] == 'Y')) {

				while (cube[5][0] != 'Y' && cube[5][4] != 'Y') {

					clockRot('Y');

				}

				Yellow_cross_alg();
				Yellow_cross_alg();

			}

			else if (cube[5][8] == 'Y') {

				Yellow_cross_alg();
				clockRot('Y');
				Yellow_cross_alg();
				Yellow_cross_alg();

			}
		}
	}

	void Yellow_corners_algorithm() {

		clockRot('G');
		clockRot('Y');
		aclockRot('G');
		clockRot('Y');
		clockRot('G');
		clockRot('Y'); clockRot('Y');
		aclockRot('G');

	}

	void Yellow_corners() {

		while (cube[5][1] != 'Y' || cube[5][3] != 'Y' || cube[5][5] != 'Y' || cube[5][7] != 'Y') {

			if ((cube[5][1] == 'Y' && cube[5][3] != 'Y' && cube[5][5] != 'Y' && cube[5][7] != 'Y')
				|| (cube[5][3] == 'Y' && cube[5][1] != 'Y' && cube[5][5] != 'Y' && cube[5][7] != 'Y')
				|| (cube[5][5] == 'Y' && cube[5][1] != 'Y' && cube[5][3] != 'Y' && cube[5][7] != 'Y')
				|| (cube[5][7] == 'Y' && cube[5][1] != 'Y' && cube[5][3] != 'Y' && cube[5][5] != 'Y'))
			{
				while (cube[5][1] != 'Y') {

					clockRot('Y');

				}

				Yellow_corners_algorithm();

			}
			else if ((cube[1][2] == 'Y' && cube[1][4] == 'Y' && cube[5][1] == 'Y' && cube[5][7] == 'Y')
				|| (cube[4][2] == 'Y' && cube[4][4] == 'Y' && cube[5][1] == 'Y' && cube[5][3] == 'Y')
				|| (cube[3][2] == 'Y' && cube[3][4] == 'Y' && cube[5][3] == 'Y' && cube[5][5] == 'Y')
				|| (cube[2][2] == 'Y' && cube[2][4] == 'Y' && cube[5][5] == 'Y' && cube[5][7] == 'Y')) {

				while (cube[2][2] != 'Y' && cube[2][4] != 'Y' && cube[5][5] != 'Y' && cube[5][7] != 'Y') {

					clockRot('Y');

				}

				Yellow_corners_algorithm();

			}
			else if ((cube[2][4] == 'Y' && cube[4][2] == 'Y' && cube[5][1] == 'Y' && cube[5][7] == 'Y')
				|| (cube[3][2] == 'Y' && cube[1][4] == 'Y' && cube[5][1] == 'Y' && cube[5][3] == 'Y')
				|| (cube[2][2] == 'Y' && cube[4][4] == 'Y' && cube[5][3] == 'Y' && cube[5][5] == 'Y')
				|| (cube[3][4] == 'Y' && cube[1][2] == 'Y' && cube[5][5] == 'Y' && cube[5][7] == 'Y'))
			{

				while (cube[2][2] != 'Y' && cube[4][4] != 'Y' && cube[5][3] != 'Y' && cube[5][5] != 'Y') {

					clockRot('Y');

				}

				Yellow_corners_algorithm();

			}
			else if ((cube[1][2] == 'Y' && cube[1][4] == 'Y' && cube[3][2] == 'Y' && cube[3][4] == 'Y')
				|| (cube[2][2] == 'Y' && cube[2][4] == 'Y' && cube[4][2] == 'Y' && cube[4][4] == 'Y')) {

				while (cube[1][2] != 'Y' && cube[1][4] != 'Y' && cube[3][2] != 'Y' && cube[3][4] != 'Y') {

					clockRot('Y');

				}

				Yellow_corners_algorithm();

			}

			else if ((cube[1][2] == 'Y' && cube[4][2] == 'Y' && cube[4][4] == 'Y' && cube[3][4] == 'Y')
				|| (cube[2][4] == 'Y' && cube[4][2] == 'Y' && cube[3][2] == 'Y' && cube[3][4] == 'Y')
				|| (cube[2][2] == 'Y' && cube[2][4] == 'Y' && cube[1][4] == 'Y' && cube[3][2] == 'Y')
				|| (cube[1][2] == 'Y' && cube[1][4] == 'Y' && cube[4][4] == 'Y' && cube[2][2] == 'Y')) {

				while (cube[1][2] != 'Y' && cube[4][2] != 'Y' && cube[4][4] != 'Y' && cube[3][4] != 'Y') {

					clockRot('Y');

				}

				Yellow_corners_algorithm();

			}

			else if ((cube[5][1] == 'Y' && cube[5][5] == 'Y' && cube[5][3] != 'Y' && cube[5][7] != 'Y')
				|| (cube[5][3] == 'Y' && cube[5][7] == 'Y' && cube[5][1] != 'Y' && cube[5][5] != 'Y')) {

				while (cube[2][2] != 'Y' && cube[1][4] != 'Y') {

					clockRot('Y');

				}

				Yellow_corners_algorithm();

			}
		}
	}

	void YCOA() {

		aclockRot('G');
		clockRot('R');
		aclockRot('G');
		clockRot('O');
		clockRot('O');
		clockRot('G');
		aclockRot('R');
		aclockRot('G');
		clockRot('O');
		clockRot('O');
		clockRot('G');
		clockRot('G');
		aclockRot('Y');

	}

	void YCO() {
		while (cube[2][2] != 'R' || cube[2][4] != 'R' || cube[1][2] != 'G' || cube[1][4] != 'G'
			|| cube[4][2] != 'O' || cube[4][4] != 'O' || cube[3][2] != 'B' || cube[3][4] != 'B') {

			if ((cube[2][2] == cube[2][4]) || (cube[1][2] == cube[1][4]) || (cube[4][2] == cube[4][4]) || (cube[3][2] == cube[3][4])) {

				while (cube[4][2] != cube[4][4]) {

					clockRot('Y');

				}

				YCOA();

				while (cube[3][2] != 'B') {

					clockRot('Y');

				}
			}

			else {

				while (cube[4][4] != 'O' && cube[2][4] != 'R') {

					clockRot('Y');

				}
				YCOA();

				while (cube[4][2] != cube[4][4]) {

					clockRot('Y');

				}

				YCOA();

				while (cube[3][2] != 'B') {

					clockRot('Y');

				}
			}
		}
	}

	void YECAR() {

		clockRot('R');
		clockRot('R');
		aclockRot('Y');
		aclockRot('G');
		clockRot('B');
		clockRot('R');
		clockRot('R');
		aclockRot('B');
		clockRot('G');
		aclockRot('Y');
		clockRot('R');
		clockRot('R');

	}

	void YECAL() {

		clockRot('R');
		clockRot('R');
		clockRot('Y');
		clockRot('B');
		aclockRot('G');
		clockRot('R');
		clockRot('R');
		aclockRot('B');
		clockRot('G');
		clockRot('Y');
		clockRot('R');
		clockRot('R');

	}

	void YECA() {

		while (cube[2][2] != 'R') {

			clockRot('R');

		}

		if (cube[2][3] == 'O' && cube[4][3] == 'R' && cube[3][3] == 'G' && cube[1][3] == 'B') {

			YECAL();

		}
		else if (cube[2][3] == 'B' && cube[3][3] == 'R') {

			YECAL();

		}
		else if (cube[2][3] == 'G' && cube[1][3] == 'R') {

			YECAL();

		}
		while (cube[4][2] != cube[4][3]) {

			clockRot('Y');

		}

		if (cube[2][3] == cube[1][2]) {

			YECAR();

		}
		else if (cube[2][3] == cube[3][2]) {

			YECAL();

		}

		while (cube[2][3] != 'R') {

			clockRot('Y');

		}
	}

	void solve() {

		cout << endl;
		cout << "we are here" << endl;
		Solve_White_cross();
		Show_cube();
		cout << "c r o s s" << endl;
		Solve_white_corners();
		Show_cube();
		cout << "c o r n e r s " << endl;
		Solve_middle_layer();
		Show_cube();
		cout << "m i d d l e" << endl;
		Yellow_cross();
		Show_cube();
		cout << "yellow c r o s s" << endl;
		Yellow_corners();
		Show_cube();
		cout << " c o r n e r s " << endl;
		YCO();
		Show_cube();
		cout << " Y C O " << endl;
		YECA();
		Show_cube();
		cout << " Y E C A " << endl;
	}

	int print_count() {
		return k - swaps;
	}


	void randomize() {

		srand(time(0));
		int rot;
		vic vc;
		swaps = rand() % 100 + 20;
		int col;
		// vc.resize(swaps);

		for (int i = 0; i < swaps; i++) {
			rot = rand() % 1;
			col = rand() % 5;

			switch (col)
			{
			case 0:
				clockRot('W');
				break;

			case 1:
				clockRot('G');
				break;

			case 2:
				clockRot('R');
				break;

			case 3:
				clockRot('B');
				break;

			case 4:
				clockRot('O');
				break;

			case 5:
				clockRot('Y');
				break;

			}

		}

		cout << '\n' << '\n' << "De_randomize:" << endl;
		cout << "De_swaps: " << swaps << endl;
		Show_cube();
		Save();
		//solve();

	}

	bool asdopfjasdop() {
		return checkCube();
	}



	//// ВИЗУАЛКА!!!!

	// номер грани для поворотов
	int current;
	void clear(double size, unsigned int* color) {
		memset(rotate, 0, sizeof(rotate));
		this->size = size;
		current = -1;

		int i, j, k;
		for (i = 0; i < 6; i++) {
			this->color[i] = color[i];
		}

		// верх
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				a[i][j][2].setColor(0, color[0]);
			}
		}
		// низ
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				a[i][j][0].setColor(1, color[1]);
			}
		}
		// спереди
		for (k = 0; k < 3; k++) {
			for (j = 0; j < 3; j++) {
				a[j][0][k].setColor(2, color[2]);
			}
		}
		// сзади
		for (k = 0; k < 3; k++) {
			for (j = 0; j < 3; j++) {
				a[j][2][k].setColor(3, color[3]);
			}
		}
		// слева
		for (i = 0; i < 3; i++) {
			for (k = 0; k < 3; k++) {
				a[0][k][i].setColor(4, color[4]);
			}
		}
		// справа
		for (i = 0; i < 3; i++) {
			for (k = 0; k < 3; k++) {
				a[2][k][i].setColor(5, color[5]);
			}
		}
		// размеры мелких деталей
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				for (k = 0; k < 3; k++) {
					a[i][j][k].size = (size / 3.0) * 0.95;
				}
			}
		}

		// прописать начальному состоянию!!!!!
		//for (int i = 0; i < 6; ++i) {
		//    std::vector <char> ch;
		//    ch = {'y', 'r', 'g', 'o', 'b', 'w'};
		//    for (int j = 0; j < 9; ++j) {
		//        cube[i][j] = ch[i];
		//    }
		//}

	}


	void draw() {
		const double K = 0.65;
		// корпус - это просто куб черного цвета, размер которого равен K*size
		glPushMatrix();
		glColor3f(0, 0, 0);
		glTranslatef(((1.0 - K) / 2) * size + K * size / 2, ((1.0 - K) / 2) * size + K * size / 2, ((1.0 - K) / 2) * size + K * size / 2);
		glutSolidCube(size * K);
		glPopMatrix();

		memset(ok, true, sizeof(ok));
		if (current != -1) {
			glPushMatrix();
			int i, j, k;

			if (current == 0 || current == 1) {
				k = (current & 1) * 2;
				for (i = 0; i < 3; i++) {
					for (j = 0; j < 3; j++) {
						ok[i][j][k] = false;
					}
				}
				glTranslated(size / 2, size / 2, 0);   // сдвигаемся к центру
				glRotatef(rotate[current], 0, 0, 1);   // крутим
				glTranslated(-size / 2, -size / 2, 0); // сдвигаемся обратно
				// рисуем
				for (i = 0; i < 3; i++) {
					for (j = 0; j < 3; j++) {
						a[i][j][k].draw(size / 3 * i, size / 3 * j, size / 3 * k);
					}
				}
			}
			else if (current == 2 || current == 3) {
				j = (current & 1) * 2;
				for (i = 0; i < 3; i++) {
					for (k = 0; k < 3; k++) {
						ok[i][j][k] = false;
					}
				}

				glTranslated(size / 2, 0, size / 2);
				glRotatef(rotate[current], 0, 1, 0);
				glTranslated(-size / 2, 0, -size / 2);
				for (i = 0; i < 3; i++) {
					for (k = 0; k < 3; k++) {
						a[i][j][k].draw(size / 3 * i, size / 3 * j, size / 3 * k);
					}
				}
			}
			else if (current == 4 || current == 5) {
				i = (current & 1) * 2;
				for (j = 0; j < 3; j++) {
					for (k = 0; k < 3; k++) {
						ok[i][j][k] = false;
					}
				}

				glTranslated(0, size / 2, size / 2);
				glRotatef(rotate[current], 1, 0, 0);
				glTranslated(0, -size / 2, -size / 2);
				for (j = 0; j < 3; j++) {
					for (k = 0; k < 3; k++) {
						a[i][j][k].draw(size / 3 * i, size / 3 * j, size / 3 * k);
					}
				}
			}
			glPopMatrix();
		}

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					if (ok[i][j][k]) {
						a[i][j][k].draw(size / 3 * i, size / 3 * j, size / 3 * k);
					}
				}
			}
		}
	}
	const int CUBE_SIZE = 13;
	int TIMER = 1;

	// проекции угла поворота на оси
	int xRot = 24;
	int yRot = 44;
	int zRot = 0;
	// отдаление
	double translateZ = -35.0;

	void display() {
		glPushMatrix();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(1, 0, 0);
		glTranslatef(0, 0, translateZ);
		glRotatef(xRot, 1, 0, 0);
		glRotatef(yRot, 0, 1, 0);
		glTranslatef(CUBE_SIZE / -2.0, CUBE_SIZE / -2.0, CUBE_SIZE / -2.0);
		draw();
		glPopMatrix();
		glutSwapBuffers();
	}

	void rot90(int idx, int sign) {
		int i, j, k;
		// sign задаётся в зависимости он направления
		// sign = -1, sign = 1
		// если sign = -1, значит крутим 3 раза
		if (sign == -1) {
			sign = 3;
		}

		while (sign--) {
			if (idx == 0) {
				k = 0;
				// копируем повёрнутую на 90 градусов верхнюю/нижнюю грань
				// в массив tmp, затем грани присваиваем tmp
				// и не забываем повернуть каждую деталь этой грани
				for (i = 0; i < 3; ++i) {
					for (j = 0; j < 3; ++j) {
						tmp[j][2 - i] = a[i][j][k];
					}
				}
				for (i = 0; i < 3; ++i) {
					for (j = 0; j < 3; ++j) {
						tmp[i][j].rotateRR();
						a[i][j][k] = tmp[i][j];
					}
				}
			}

			else if (idx == 1) {
				k = 2;
				for (i = 0; i < 3; ++i) {
					for (j = 0; j < 3; ++j) {
						tmp[j][2 - i] = a[i][j][k];
					}
				}
				for (i = 0; i < 3; ++i) {
					for (j = 0; j < 3; ++j) {
						tmp[i][j].rotateRR();
						a[i][j][k] = tmp[i][j];
					}
				}
			}

			else if (idx == 2) {
				j = 0;
				for (i = 0; i < 3; ++i)
					for (k = 0; k < 3; ++k)
						tmp[k][2 - i] = a[i][j][k];
				for (i = 0; i < 3; ++i) {
					for (k = 0; k < 3; ++k) {
						tmp[i][k].rotateGD();
						a[i][j][k] = tmp[i][k];
					}
				}
			}

			else if (idx == 3) {
				j = 2;
				for (i = 0; i < 3; ++i)
					for (k = 0; k < 3; ++k)
						tmp[k][2 - i] = a[i][j][k];
				for (i = 0; i < 3; ++i) {
					for (k = 0; k < 3; ++k) {
						tmp[i][k].rotateGD();
						a[i][j][k] = tmp[i][k];
					}
				}
			}

			else if (idx == 4) {
				i = 0;
				for (j = 0; j < 3; ++j) {
					for (k = 0; k < 3; ++k) {
						tmp[k][2 - j] = a[i][j][k];
					}
				}
				for (j = 0; j < 3; ++j) {
					for (k = 0; k < 3; ++k) {
						tmp[j][k].rotateGR();
						a[i][j][k] = tmp[j][k];
					}
				}
			}

			else if (idx == 5) {
				i = 2;
				for (j = 0; j < 3; ++j) {
					for (k = 0; k < 3; ++k) {
						tmp[k][2 - j] = a[i][j][k];
					}
				}
				for (j = 0; j < 3; ++j) {
					for (k = 0; k < 3; ++k) {
						tmp[j][k].rotateGR();
						a[i][j][k] = tmp[j][k];
					}
				}
			}
		}
	}

	void timer(int RorL, int) {
		if (current != -1) {
			display();
			Rotate(current, 3, RorL);
		}
	}
	void Rotate(int idx, int angle, int RorL) {
		if (current == -1 || current == idx) {
			if (RorL == -1) {
				rotate[idx] -= angle;
			}
			else if (RorL == 1) {
				rotate[idx] += angle;
			}

			if (rotate[idx] % 90 != 0) {
				current = idx;
				timer(RorL, 0);
			}
			else {
				// угол стал кратным 90, поварачиваем на массиве
				if ((rotate[idx] < 0) ^ (current == 2 || current == 3)) {
					rot90(idx, 1);
				}
				else {
					rot90(idx, -1);
				}
				rotate[idx] = 0;
				current = -1;
			}
		}
	}
};


const int CUBE_SIZE = 13;
const int TIMER = 1;
// обозначаем цвета
unsigned int color[6] = { 0x00FF00, 0x0000FF, 0xFFFF00, 0xFFFFFF, 0xFF6800, 0xFF0000 };

//0xFFFF00 - yellow
//0x0000FF - blue
//0x00FF00 - green
//0xFF0000 - red
//0xFF6800- orange
//0xFFFFFF - white

// проекции угла поворота на оси
int xRot = 24;
int yRot = 44;
int zRot = 0;
// отдаление
double translateZ = -35.0;

Ccube kubik;

void display() {
	glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 0, 0);
	glTranslatef(0, 0, translateZ);
	glRotatef(xRot, 1, 0, 0);
	glRotatef(yRot, 0, 1, 0);
	glTranslatef(CUBE_SIZE / -2.0, CUBE_SIZE / -2.0, CUBE_SIZE / -2.0);
	kubik.draw();
	glPopMatrix();
	glutSwapBuffers();
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat fAspect = (GLfloat)w / (GLfloat)h;
	gluPerspective(60, fAspect, 1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void init() {
	// цвет фона
	glClearColor(0.482, 0.408, 0.933, 0.0);

	// освещение
	float mat_specular[] = { 0.3, 0.3, 0.3, 0 };
	float diffuseLight[] = { 0.2, 0.2, 0.2, 1 };
	float ambientLight[] = { 0.9, 0.9, 0.9, 1.0 };
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	// инициализируем куб
	kubik.clear(CUBE_SIZE, color);
}


void specialKeys(int key, int, int) {

	if (key == GLUT_KEY_DOWN) {
		xRot += 3;
		if (xRot >= 360)
			xRot -= 360;
		glutPostRedisplay();
	}

	if (key == GLUT_KEY_UP) {
		xRot -= 3;
		if (xRot < 0)
			xRot += 360;
		glutPostRedisplay();
	}

	if (key == GLUT_KEY_RIGHT) {
		yRot += 3;
		if (yRot >= 360)
			yRot -= 360;
		glutPostRedisplay();
	}

	if (key == GLUT_KEY_LEFT) {
		yRot -= 3;
		if (yRot < 0)
			yRot += 360;
		glutPostRedisplay();
	}

	if (key == GLUT_KEY_HOME) {
		translateZ += 5;
		glutPostRedisplay();
	}

	if (key == GLUT_KEY_END) {
		translateZ -= 5;
		glutPostRedisplay();
	}


	if (key == GLUT_KEY_F1) {
		kubik.clear(CUBE_SIZE, color);
		glutPostRedisplay();
	}

}


void keys(unsigned char key, int, int) {
	if (kubik.current == -1 && key == 'W') {
		kubik.rot('W');
	}
	else if (kubik.current == -1 && key == 'R') {
		kubik.rot('R');
	}
	else if (kubik.current == -1 && key == 'Y') {
		kubik.rot('Y');
	}
	else if (kubik.current == -1 && key == 'O') {
		kubik.rot('O');
	}
	else if (kubik.current == -1 && key == 'G') {
		kubik.rot('G');
	}
	else if (kubik.current == -1 && key == 'B') {
		kubik.rot('B');
	}
}

void timer(int) {
	glutTimerFunc(TIMER, timer, 0);
	if (kubik.current != -1) {
		kubik.Rotate(kubik.current, 3, -1);
	}
	display();
}

void processMenuPatterns(int option) {
	switch (option) {
	}
}

void processMenu(int option) {
	switch (option) {
	case 1:
		kubik.randomize();
		break;
	case 2:
		kubik.solve();
		break;
	case 3:
		cout << '\n' << "Nikolay Kondratev m3111" << '\n' << "email is djhitekdieant@gmail.com" << '\n' << "Git: " << '\n' << "https://github.com/Kornely-little-dev" << endl;
		break;
	case 4:
		cout << '\u2344' << endl;
		wcout << ".⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠰⣤⣤⢀" << '\n'
			<< "⠄⠄⠄⠄⠄⠄⠄⣀⣀⣀⠄⠄⢿⣿⣦⠈⢄" << '\n'
			<< "⠄⠄⠄⠄⠄⠈⠻⣿⣶⣦⣍⠐⢼⣿⣿⣧⠈⡄" << '\n'
			<< "⠄⠄⠄⠄⠄⠄⠄⠈⢿⣿⣿⣷⣄⣿⣿⣿⣦⣴⣦⣀" << '\n'
			<< "⠄⠄⠄⠄⢀⢠⣄⣒⣚⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡧" << '\n'
			<< "⠄⠄⠄⠰⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡧⣤⣤⣤⣄" << '\n'
			<< "⠄⠄⠄⠄⠄⠄⠉⠻⢿⣿⣿⣿⡽⣿⠟⢃⣿⠟⡉⣿⣿⣿⣿⣿⠿⠛⠉" << '\n'
			<< "⠄⠄⠄⠄⠄⠠⠶⢯⣭⣿⢹⡟⢶⣗⠄⢸⠇⢁⣠⠫⠟⣹⣿⣯⡭⠶⠂" << '\n'
			<< "⠄⠄⠄⠄⠄⠄⠄⠄⠈⢙⣄⠣⠁⠈⠓⠊⠐⠃⠄⠄⣰⣿⣏⡀" << '\n'
			<< "⠄⠄⠄⠄⠄⠄⠄⠄⠄⠉⢉⣽⠄⠄⠄⠄⠄⠄⠄⣿⣯⠉⠉" << '\n'
			<< "⠄⠄⠄⠄⠄⠄⠄⠄⠄⣠⣿⣿⡀⠄⠄⠄⠄⠄⠄⣿⣿⣷⡀" << '\n'
			<< "⠄⠄⠄⠄⠄⠄⢀⣴⣾⣿⣿⣿⣷⡀⠄⠄⠄⠄⢠⣿⣿⣿⣷⣦⣄⡀" << '\n'
			<< "⠄⠄⠄⠄⣰⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⡀" << '\n'
			<< "⠄⠄⢀⠘⢛⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⢿⣿⣿⣿⠗⠄" << '\n'
			<< "⠄⢀⣧⣀⡀⠶⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⣠⣀⣀⣿⡿⠃⢢⣠⡆" << '\n'
			<< "⠄⢸⠿⣿⡿⣷⡿⠿⣿⢿⡿⠩⢹⣿⡟⠉⠉⠉⢹⣿⢻⠛⠛⠻⢿⣿⣷⣿⣿⠉⠟" << '\n'
			<< "⠄⡾⢶⣿⣦⣿⣷⡇⢁⠚⡇⠐⢀⣿⣷⣤⣤⣤⣿⣧⣀⣠⣇⡰⢀⡿⣿⣿⣷⣾⣶⡀" << '\n'
			<< "⢰⣵⡎⠹⣿⣿⣿⣁⣾⢻⣿⢻⣟⣩⣿⣿⣿⣿⣿⣿⣿⣿⣿⣥⣾⣿⡿⠛⠛⢛⡟⢇" << '\n'
			<< "⣾⣿⣞⣰⠞⣩⣿⣻⡇⢠⠃⠸⠉⠙⣿⣿⣿⣿⢈⡁⠤⠁⠌⢉⢙⣿⣐⣧⣤⣾⢇⢸" << '\n'
			<< "⢹⡿⢿⢋⡍⢇⠛⣿⣧⣼⣦⣴⣔⣠⣿⣿⣿⣯⣦⣤⣦⣤⣦⣄⣸⡻⢛⠋⠉⠻⠿⡇" << '\n'
			<< "⠄⡰⠄⣆⣷⣴⣶⠿⣿⡿⣿⢿⣿⣿⣿⣿⣿⢿⡿⢿⣿⢉⠉⣿⣿⣷⣾⣤⣭⡁⠂⠄" << '\n'
			<< "⠄⢱⣾⣿⣿⣿⣿⠐⣉⣌⠇⠐⠁⡄⣿⣿⣿⠈⡠⢾⡇⠠⠠⣼⣽⡟⢣⠄⣹⣿⡖" << '\n'
			<< "⠄⠄⢿⣉⠁⣿⣷⣿⡟⢿⡵⣾⣶⣦⣿⣿⣿⣴⣷⣾⣷⠖⡴⠿⣿⣿⠷⠚⢛⡻⠁" << '\n'
			<< "⠄⠄⠈⢿⣆⣿⣿⣿⣧⠐⡅⢘⠃⣿⣿⣿⣟⠿⡿⣿⣧⣈⠴⢃⠘⣻⣿⣿⡟" << '\n'
			<< "⠄⠄⠄⢰⢻⣿⠃⢸⣿⣶⣿⡿⡟⢉⣿⢿⣻⣼⣧⠂⠛⢿⣷⣟⣀⣥⡼⠋" << '\n'
			<< "⠄⠄⠄⣾⣏⡙⣿⣾⣿⣿⠿⢄⠉⣸⡿⠁⠉⠉⠟⢬⢛⢚⠻⣿⣿⠛" << '\n'
			<< "⠄⠄⠄⣿⣷⣦⣅⣙⣿⣧⣮⣬⣽⡟⠄⠄⠄⠄⢶⣶⣮⢧⢠⠙⣰⢹⣦" << '\n'
			<< "⠄⠄⠄⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⡀⠄⠄⠄⠄⠄⣈⣽⣶⣿⣿⣷⣼⠟⠆" << '\n'
			<< "⠄⠄⠄⠘⢥⣏⢿⢯⣿⡟⠄⣿⣿⣿⣶⣄⣠⢈⠲⠟⣛⠏⣍⣏⠬⣄⡺" << '\n'
			<< "⠄⠄⠄⠄⢸⣿⣿⣾⣿⣃⣧⣿⣿⣿⣿⣿⣧⣒⣴⣦⣾⣶⣾⣶⣿⡇" << endl;

		break;
	}
}

void createGLUTMenus() {
	glutCreateMenu(processMenu);

	glutAddMenuEntry("Случайное положение", 1);
	glutAddMenuEntry("Сборка текущего состояния", 2);
	glutAddMenuEntry("Доп. информация", 3);
	glutAddMenuEntry("flex", 4);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv) {

	kubik.Resize_cube();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 500);
	glutInitWindowPosition(1320, 550);
	glutCreateWindow("Пожилая черепашка");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keys);
	glutTimerFunc(TIMER, timer, 0);
	glutSpecialFunc(specialKeys);
	createGLUTMenus();
	glutMainLoop();

	return 0;
}

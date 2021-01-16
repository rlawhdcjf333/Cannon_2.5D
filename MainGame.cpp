#include "pch.h"
#include "MainGame.h"

#include "Image.h"

#define PI 3.141592

/*
Initialize : �ʱ�ȭ
������ ������ �����ϱ� ���� �ʱ�ȭ �� ������ ���⼭ ����
���� �����ϱ� ���� �� �ѹ��� ����Ǵ� �Լ�
*/
void MainGame::Init()
{
	SetTimer(_hWnd, 1, 10, NULL);

	mBackBuffer = new Image();
	mBackBuffer->CreateEmpty(WINSIZEX, WINSIZEY);

	mCannon.x = WINSIZEX / 2;
	mCannon.y = WINSIZEY;
	mCannon.angle = PI / 2;
	mCannon.connonLength = 105.f;
	mCannon.radius = 50.f;

	target.x = WINSIZEX / 2;
	target.y = 300;
	target.radius = (target.y+700) * 0.025;

	mGravity = 0.98f;

	cannonPow = 21;

	mDegree = PI / 2;

	wind = 0;
	wind = -0.37f;

	frameRate = 0;

	score = 0;


	for (int i = 0; i < 10; i++) {

		windrc[i] = RectMakeCenter(WINSIZEX / 2 - 90 + 20 * i, WINSIZEY / 2 + 80, 20, 10);

	}

}

/*
Release : �޸� ������ �� �ҷ��ִ� �Լ�
����Ƽ��� OnDestroy
�ش� Ŭ���� �ν��Ͻ��� �޸� ���� �� �� �� �ѹ� ȣ�����ִ� �༮
*/
void MainGame::Release()
{
	KillTimer(_hWnd, 1);//Ÿ�̸� ������

	Random::ReleaseInstance();	//�̱��� �ν��Ͻ� ����

	delete mBackBuffer;
	mBackBuffer = NULL;
}

/*
Update : �� ������ ����Ǵ� �Լ�, ���⼭ ���� ó�� �Ѵ�.
*/
void MainGame::Update()
{
	frameRate ++;
	//canon shooting power control
	if (Input::GetInstance()->GetKey('W') and cannonPow <= 40) 
	{ if(Input::GetInstance()->GetKey(VK_SHIFT)) { cannonPow +=0.025; } else cannonPow += 0.25; }
	if (Input::GetInstance()->GetKey('S') and cannonPow >= 20.25) 
	{ if (Input::GetInstance()->GetKey(VK_SHIFT)) { cannonPow -= 0.025; } else cannonPow -= 0.25; }
	//cannon angle control
	if (Input::GetInstance()->GetKey('A') and mDegree < PI - 0.2) 
	{ if (Input::GetInstance()->GetKey(VK_SHIFT)) {mDegree += 0.001f;} else mDegree += 0.01f;}
	if (Input::GetInstance()->GetKey('D') and mDegree > 0.2) 
	{ if (Input::GetInstance()->GetKey(VK_SHIFT)) { mDegree -= 0.001f;} else mDegree -= 0.01f;}

	if (mCannon.connonLength < cannonPow * 5) {
		mCannon.connonLength += 1;
	}
	if (mCannon.connonLength > cannonPow * 5) {
		mCannon.connonLength -= 1;
	}

	mCannon.cannonEndX = mCannon.x+cosf(mDegree) * mCannon.connonLength; //cannon angle movement
	mCannon.cannonEndY = mCannon.y-sinf(mDegree) * mCannon.connonLength;

	if (Input::GetInstance()->GetKeyDown(VK_SPACE) and mCannon.connonLength>100) //firing action
	{
		X.push_back({ mCannon.cannonEndX, mCannon.cannonEndY, 30, mDegree, -cannonPow * sinf(mDegree),cannonPow * cosf(mDegree)});//bullet starting physics set

		mCannon.connonLength -= cannonPow * 2; //gun barrel back movement
	}

	for (int i = 0; i < X.size(); i++) { //bullet physics

		X[i].x += X[i].xvector;
		X[i].xvector += wind; //trajectory moving object;s x-vector element 
		X[i].y += X[i].antiG;
		X[i].antiG += mGravity; //trajectory moving object's y-vector element

		X[i].radius -= 0.25;

		if (X[i].x<=0 or X[i].x>=WINSIZEX or X[i].y >= WINSIZEY or X[i].radius<15.5) { //erase out-of-window object


			X.erase(X.begin() + i);
			X.shrink_to_fit();
		}

	}

	for (int i = 0; i < 20; i++) { //trajectory curve approximation (definite integral, C=0)

		dots[i].x = mCannon.cannonEndX + cannonPow * cosf(mDegree) * 3 * i + wind*0.5 * powf(3 * i, 2);
		dots[i].y = mCannon.cannonEndY - cannonPow * sinf(mDegree) * 3*i+ mGravity *0.5* powf(3*i,2);
		dots[i].radius = 5;
	}

	for (int i = 0; i < X.size(); i++) { //target-bullet collision check

		X[i].dist = sqrtf(powf(target.x - X[i].x, 2) + powf(target.y - X[i].y, 2));
	}

	for (int i = 0; i < X.size(); i++) {
		if (X[i].dist < target.radius+X[i].radius and X[i].radius<15.75) {//get target trigger; wind vector change; target position change; scoring; bullet erase;


			if (wind < 0) { wind = Random::GetInstance()->RandomInt(5, 50) * 0.01; }
			else { wind = -Random::GetInstance()->RandomInt(5, 50) * 0.01; }

			target.x = Random::GetInstance()->RandomInt(50, WINSIZEX - 50);
			target.y = Random::GetInstance()->RandomInt(50, WINSIZEY-200);
			target.radius = (target.y+700) * 0.025;
			X.erase(X.begin() + i);
			X.shrink_to_fit();

			score++;
		}
	}
	//for (int i = 0; i < X.size(); i++) {
	//	if (X[i].radius < 15.75) {
	//
	//		X[i].effect = RectMakeCenter(X[i].x, X[i].y, 40, 40);
	//	}
	//}



}

/*
Render : �� ������ ����Ǵ� �Լ�, Update�� ������ Render�� ����ȴ�.
ȭ�鿡 �׷��ִ� �͵��� ���� ���⼭ ó��

�Ű����� hdc : ������ â�� HDC�� ����
*/
void MainGame::Render(HDC hdc)
{
	//������� HDC �����´�
	HDC backDC = mBackBuffer->GetHDC();
	//HDC ������ Ư�� ������ �о������ �༮
	PatBlt(backDC, 0, 0, WINSIZEX, WINSIZEY, WHITENESS);
	// ==================================================
	{
		HBRUSH color;
		HBRUSH white;
		HPEN newP;
		HPEN oldP;
		SetBkMode(backDC, TRANSPARENT);

		for (int i = 0; i < 100; i++) { //bkground 2.5D effect
			color = CreateSolidBrush(RGB(0, 2*i, 0));
			white = (HBRUSH)SelectObject(backDC, color);
			newP = CreatePen(PS_ENDCAP_MASK, 0, RGB(0, 2*i, 0));
			oldP = (HPEN)SelectObject(backDC, newP);

			RenderRect(backDC, RectMakeCenter(WINSIZEX / 2, 8*i, WINSIZEX, WINSIZEY/99+1));

			SelectObject(backDC, oldP);
			SelectObject(backDC, white);
			DeleteObject(color);
			DeleteObject(newP);
		}

		for (int i = 0; i < X.size(); i++) { //explosion bkground reaction render
			if (X[i].radius < 16) {
					color = CreateSolidBrush(RGB(255, 0, 0));
					white = (HBRUSH)SelectObject(backDC, color);
					RenderRect(backDC, RectMake(0, 0, WINSIZEX, WINSIZEY));
					SelectObject(backDC, white);
					DeleteObject(color);
			}
		}

		RenderLine(backDC, 0, WINSIZEY, 300, 0); //2.5D linear effect
		RenderLine(backDC, WINSIZEX, WINSIZEY, WINSIZEX-300, 0);

		wstring scoreT = L"Score : " + to_wstring(score);
		TextOut(backDC, WINSIZEX / 2 - 25, WINSIZEY / 2 - 250, scoreT.c_str(), scoreT.length());

		wstring exp1 = L"����: WASD";
		TextOut(backDC, 100, WINSIZEY / 2 - 100, exp1.c_str(), exp1.length());

		wstring exp = L"��������: SHIFT + WASD";
		TextOut(backDC, 100, WINSIZEY / 2-50, exp.c_str(), exp.length());

		wstring cP = L"���� �߻� �� " + to_wstring(cannonPow*1.2);
		TextOut(backDC, 100 , WINSIZEY / 2, cP.c_str(), cP.length());

		wstring sT = L"��� : SPACE ";
		TextOut(backDC, 100, WINSIZEY / 2+50, sT.c_str(), sT.length());

		wstring caution = L"!! ���� ���� �ٶ� !! ";
		TextOut(backDC, 100, WINSIZEY / 2 + 100, caution.c_str(), caution.length());

		wstring windT = L"ǳ�� �� ǳ��";
		TextOut(backDC, 600, WINSIZEY / 2+50, windT.c_str(), windT.length());
		
		if (wind < 0) { //wind level visulization render (plus vector)
			for (int i = 4; i > 4+wind*10; i--) {
		
				color = CreateSolidBrush(RGB(255, 0, 0));
				white = (HBRUSH)SelectObject(backDC, color);
		
				RenderRect(backDC, windrc[i]);
		
				SelectObject(backDC, white);
				DeleteObject(color);
		
			}
		}
		
		if (wind > 0) { //wind level visualization render (minus vector)
			for (int i = 5; i < 5+wind * 10; i++) {
		
				color = CreateSolidBrush(RGB(0, 255, 0));
				white = (HBRUSH)SelectObject(backDC, color);
		
				RenderRect(backDC, windrc[i]);
		
				SelectObject(backDC, white);
				DeleteObject(color);
		
			}
		}

		RenderLine(backDC, mCannon.x, mCannon.y, mCannon.cannonEndX, mCannon.cannonEndY);//canon body render
		RenderEllipse(backDC, mCannon.x, mCannon.y, mCannon.radius);

		color = CreateSolidBrush(RGB((int)frameRate%60+180, (int)frameRate%60+180, 100)); //target body render
		white = (HBRUSH)SelectObject(backDC, color);

		RenderEllipse(backDC, target.x, target.y, target.radius);

		SelectObject(backDC, white);
		DeleteObject(color);

		for (int i = 0; i < X.size(); i++) {
			if (X[i].dist < target.radius+X[i].radius+10 and X[i].radius<17) { //target approach signal render
				color = CreateSolidBrush(RGB(255, 0, 0));
				white = (HBRUSH)SelectObject(backDC, color);

				RenderEllipse(backDC, target.x, target.y, target.radius);

				SelectObject(backDC, white);
				DeleteObject(color);

			}
		}

		for (int i = 0; i < X.size(); i++) { //bullet body render

			color = CreateSolidBrush(RGB(120, 120, 120));
			white =(HBRUSH) SelectObject(backDC, color);
			
			RenderEllipse(backDC, X[i].x, X[i].y, X[i].radius);

			SelectObject(backDC, white);
			DeleteObject(color);

		}

		for (int i = 1; i < 20; i++) { //trajectory line render

			color = CreateSolidBrush(RGB(120, 200, 120));
			white = (HBRUSH)SelectObject(backDC, color);

			RenderEllipse(backDC, dots[i].x, dots[i].y, dots[i].radius);

			SelectObject(backDC, white);
			DeleteObject(color);
		}

		for (int i = 0; i < X.size(); i++) { //bullet explosion effect render
			color = CreateSolidBrush(RGB(255, 20, 20));
			white = (HBRUSH)SelectObject(backDC, color);
			RenderRect(backDC, X[i].effect);
			SelectObject(backDC, white);
			DeleteObject(color);
		}

		wstring overheat = L"���� : ����";
		if (mCannon.connonLength < 100) { 
			overheat = L"���� : ����!"; 

			color = CreateSolidBrush(RGB(255, 0, 0));
			white = (HBRUSH)SelectObject(backDC, color);
			RenderEllipse(backDC, mCannon.x, mCannon.y, mCannon.radius);
			SelectObject(backDC, white);
			DeleteObject(color);
		}
		HFONT newF = CreateFontW(40, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, NULL);
		HFONT oldF = (HFONT)SelectObject(backDC, newF);
		TextOut(backDC, 90, WINSIZEY / 2 - 180, overheat.c_str(), overheat.size());
		SelectObject(backDC, oldF);
		DeleteObject(newF);

		

	}
	//====================================================
	//�ĸ���� ������ ������ â�� ��� ����
	mBackBuffer->Render(hdc, 0, 0);
}

#include "MyFirstWndGame.h"
#include "GameTimer.h"
#include "Collider.h"
#include "GameObject.h"
#include "RenderHelp.h"
#include "INC_Windows.h"
#include <iostream>
#include <assert.h>

#include <string>
#include "string.h"


using namespace learning;

constexpr int MAX_GAME_OBJECT_COUNT = 1000;
int i = 0;//test


bool MyFirstWndGame::Initialize()
{
    m_pGameTimer = new GameTimer();
    m_pGameTimer->Reset();

    const wchar_t* className = L"MyFirstWndGame";
    const wchar_t* windowName = L"MyFirstWndGame";

    if (false == __super::Create(className, windowName, 1024, 720))
    {
        return false;
    }
    


    RECT rcClient = {};
    GetClientRect(m_hWnd, &rcClient);
    m_width = rcClient.right - rcClient.left;
    m_height = rcClient.bottom - rcClient.top;

    m_hFrontDC = GetDC(m_hWnd);
    m_hBackDC = CreateCompatibleDC(m_hFrontDC);
    m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);

    m_hDefaultBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);

    m_GameObjectPtrTable = new GameObjectBase * [MAX_GAME_OBJECT_COUNT];

    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        m_GameObjectPtrTable[i] = nullptr;
    }
#pragma region resource
    m_pPlayerBitmapInfo = renderHelp::CreateBitmapInfo(L"../Resource/redbird.png");
    m_pEnemyBitmapInfo = renderHelp::CreateBitmapInfo(L"../Resource/graybird.png");

    if (m_pPlayerBitmapInfo == nullptr || m_pEnemyBitmapInfo == nullptr)
    {
        std::cout << "Bitmap Load Failed!" << std::endl;
        return false;
    }

#pragma endregion
    // [CHECK]. 첫 번째 게임 오브젝트는 플레이어 캐릭터로 고정!
    CreatePlayer();

    return true;

}




void MyFirstWndGame::Run()
{
    MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_LBUTTONDOWN)
            {
                MyFirstWndGame::OnLButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
                //SYtest Start
                std::wstring NewClassName = L"SYFirstWndGame" + std::to_wstring(i);
                std::wstring NewWindowName = L"SYFirestWndGame" + std::to_wstring(i);

                const wchar_t* SYclassName = NewClassName.c_str();
                const wchar_t* SYwindowName = NewWindowName.c_str();
                i++;
                __super::Create(SYclassName, SYwindowName, 800, 600);
                
                //SYtest End
            }
            else if (msg.message == WM_RBUTTONDOWN)
            {
                MyFirstWndGame::OnRButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else if (msg.message == WM_MOUSEMOVE)
            {
                MyFirstWndGame::OnMouseMove(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            Update();
            Render();
        }
    }
}

void MyFirstWndGame::Finalize()
{
    delete m_pGameTimer;
    m_pGameTimer = nullptr;

    if (m_GameObjectPtrTable)
    {
        for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
        {
            if (m_GameObjectPtrTable[i])
            {
                delete m_GameObjectPtrTable[i];
                m_GameObjectPtrTable[i] = nullptr;
            }
        }
        delete m_GameObjectPtrTable;
    }

    __super::Destroy();
}

void MyFirstWndGame::FixedUpdate()
{
    if (m_EnemySpawnPos.x != 0 && m_EnemySpawnPos.y != 0)
    {
        CreateEnemy();
    }
}

void MyFirstWndGame::LogicUpdate()
{

    UpdatePlayerInfo();
    
    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        if (m_GameObjectPtrTable[i])
        {
            m_GameObjectPtrTable[i]->Update(m_fDeltaTime);
            UpdateEnemyInfo((GameObject*)m_GameObjectPtrTable[i]);
        }
    }
}

void MyFirstWndGame::CreatePlayer()
{
    assert(m_GameObjectPtrTable[0] == nullptr && "Player object already exists!");

    GameObject* pNewObject = new GameObject(ObjectType::PLAYER);

    pNewObject->SetName("Player");
    pNewObject->SetPosition(0.0f, 0.0f); // 일단, 임의로 설정 
    pNewObject->SetSpeed(1.0f); // 일단, 임의로 설정   
    pNewObject->SetWidth(100);
    pNewObject->SetHeight(100);
   


    pNewObject->SetColliderCircle(50.0f); // 일단, 임의로 설정. 오브젝트 설정할 거 다 하고 나서 하자.
    pNewObject->SetBitmapInfo(m_pPlayerBitmapInfo);
    m_GameObjectPtrTable[0] = pNewObject;
}

void MyFirstWndGame::CreateEnemy()
{
    GameObject* pNewObject = new GameObject(ObjectType::ENEMY);

    pNewObject->SetName("Enemy");

    float x = m_EnemySpawnPos.x;
    float y = m_EnemySpawnPos.y;

    m_EnemySpawnPos = { 0, 0 };//다시 초기화
  
    pNewObject->SetPosition(x, y);
    pNewObject->SetSpeed(0.1f); // 일단, 임의로 설정   

    pNewObject->SetWidth(100);
    pNewObject->SetHeight(100);
    pNewObject->SetBitmapInfo(m_pEnemyBitmapInfo);

    pNewObject->SetColliderCircle(50.0f); // 일단, 임의로 설정. 오브젝트 설정할 거 다 하고 나서 하자.
    








    Vector2f newIndex = { pNewObject->GetPosition().x,pNewObject->GetPosition().y };

    int i = 0;
    while (++i < MAX_GAME_OBJECT_COUNT) //0번째는 언제나 플레이어!
    {
        
        if (nullptr != m_GameObjectPtrTable[i])
        {
            Vector2f oldIndex = { m_GameObjectPtrTable[i]->GetPosition().x ,m_GameObjectPtrTable[i]->GetPosition().y };
            if ((newIndex - oldIndex).LengthSquared() <= pow(100,2))
            {
                std::cout << "겹침;;;; 벌써" <<i<<"번째임;;;"<< std::endl;
                break;
            }
        }
        
        

        if (nullptr == m_GameObjectPtrTable[i])
        {
            m_GameObjectPtrTable[i] = pNewObject;
            break;
        }
    }

    if (i == MAX_GAME_OBJECT_COUNT)
    {
        // 게임 오브젝트 테이블이 가득 찼습니다.
        delete pNewObject;
        pNewObject = nullptr;
    }
}

void MyFirstWndGame::UpdatePlayerInfo()
{
    static GameObject* pPlayer = GetPlayer();

    assert(pPlayer != nullptr);

    Vector2f mousePos(m_PlayerTargetPos.x, m_PlayerTargetPos.y);
    Vector2f playerPos = pPlayer->GetPosition();
    Vector2f playerDir = mousePos - playerPos;
    float distance = playerDir.Length(); // 거리 계산



    if (distance > 50.f) //임의로 설정한 거리
    {
        playerDir.Normalize(); // 정규화
        pPlayer->SetDirection(playerDir); // 플레이어 방향 설정
    }
    else
    {
        pPlayer->SetDirection(Vector2f(0, 0)); // 플레이어 정지
    }
}
void MyFirstWndGame::UpdateEnemyInfo(GameObject* Enemy)
{
    static GameObject* pPlayer = GetPlayer();

    assert(pPlayer != nullptr);

    Vector2f playerPos = pPlayer->GetPosition();
    Vector2f playerDir = playerPos - Enemy->GetPosition();
    float distance = playerDir.Length(); // 거리 계산
    if (distance > 0.0f)
    {
        playerDir.Normalize();
    }

    bool isavoiding = false;

    for (int i = 1; i < MAX_GAME_OBJECT_COUNT; i++)
    {
        if (m_GameObjectPtrTable[i] == Enemy) continue;//자기면 검사 통과
        if (m_GameObjectPtrTable[i] != nullptr)
        {
            bool isinteract = Intersect(Enemy->GetColliderCircle(), ((GameObject*)m_GameObjectPtrTable[i])->GetColliderCircle());
            if (isinteract)
            {
                Vector2f avoid = Enemy->GetPosition() - ((GameObject*)m_GameObjectPtrTable[i])->GetPosition();

                if (avoid.Length() > 0.0f)
                {
                    avoid.Normalize();
                    playerDir = playerDir + avoid * 5.0f;
                    isavoiding = true;
                }
            }
        }
        
    }

    if (distance > 100.f) //임의로 설정한 거리
    {
        playerDir.Normalize(); // 정규화
        Enemy->SetDirection(playerDir); // 플레이어 방향 설정
    }
    else if(Intersect(Enemy->GetColliderCircle(), pPlayer->GetColliderCircle()))
    {
        
            Enemy->SetDirection(Vector2f(0, 0));
        
    }
}

void MyFirstWndGame::Update()
{
    m_pGameTimer->Tick();

    LogicUpdate();

    m_fDeltaTime = m_pGameTimer->DeltaTimeMS();
    m_fFrameCount += m_fDeltaTime;

    while (m_fFrameCount >= 200.0f)
    {
        FixedUpdate();
        m_fFrameCount -= 200.0f;
    }
}

void MyFirstWndGame::Render()
{
    //Clear the back buffer
    ::PatBlt(m_hBackDC, 0, 0, m_width, m_height, WHITENESS);

    
    //메모리 DC에 그리기
    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        bool Crash = false;
        if (m_GameObjectPtrTable[i] != nullptr && i != 0)
        {
            Crash = Intersect(GetPlayer()->GetColliderCircle(), ((GameObject*)m_GameObjectPtrTable[i])->GetColliderCircle());
        }

        if (m_GameObjectPtrTable[i])
        {
            m_GameObjectPtrTable[i]->Render(m_hBackDC, Crash);
            if (Crash)
            {
                m_GameObjectPtrTable[0]->Render(m_hBackDC, Crash);
            }
        }
    }
    
    
    //메모리 DC에 그려진 결과를 실제 DC(m_hFrontDC)로 복사
    BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);
}

void MyFirstWndGame::OnResize(int width, int height)
{
    std::cout << __FUNCTION__ << std::endl;

    learning::SetScreenSize(width, height);

    __super::OnResize(width, height);

    m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);

    HANDLE hPrevBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);

    DeleteObject(hPrevBitmap);
}

void MyFirstWndGame::OnClose()
{
    std::cout << __FUNCTION__ << std::endl;

    SelectObject(m_hBackDC, m_hDefaultBitmap);

    DeleteObject(m_hBackBitmap);
    DeleteDC(m_hBackDC);

    ReleaseDC(m_hWnd, m_hFrontDC);
}

void MyFirstWndGame::OnMouseMove(int x, int y)
{
    /*   std::cout << __FUNCTION__ << std::endl;
       std::cout << "x: " << x << ", y: " << y << std::endl;*/
    m_MousePosPrev = m_MousePos;
    m_MousePos = { x, y };
}

void MyFirstWndGame::OnLButtonDown(int x, int y)
{
    /*  std::cout << __FUNCTION__ << std::endl;
 std::cout << "x: " << x << ", y: " << y << std::endl;*/

    m_PlayerTargetPos.x = x;
    m_PlayerTargetPos.y = y;

}

void MyFirstWndGame::OnRButtonDown(int x, int y)
{
    /*  std::cout << __FUNCTION__ << std::endl;
   std::cout << "x: " << x << ", y: " << y << std::endl;*/

    m_EnemySpawnPos.x = x;
    m_EnemySpawnPos.y = y;
}
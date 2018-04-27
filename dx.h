#ifndef DX_H
#define DX_H

#include <QElapsedTimer>

#include <d3d11.h>
#include <dxgi.h>

#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")

class DX
{
public:
    QImage                      bitmap;
    int                         FPS = 0;

    bool resizeBuffers(int w, int h)
    {
        windowWidth = w;
        windowHeight = h;

        if (swapchain) {
            context->OMSetRenderTargets(0, 0, 0);

            HRESULT hr = swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

            ID3D11Texture2D *pBackBuffer;
            hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

            hr = device->CreateRenderTargetView(pBackBuffer, NULL, &backbufferView);
            pBackBuffer->Release();

            D3D11_VIEWPORT viewport;
            viewport = { 0 };
            viewport.TopLeftX = 0;
            viewport.TopLeftY = 0;
            viewport.Width = w;
            viewport.Height = h;
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;

            context->OMSetRenderTargets(1, &backbufferView, depthStencilView);
            context->RSSetViewports(1, &viewport);

            D3D11_RASTERIZER_DESC rasterDesc;
            rasterDesc.AntialiasedLineEnable = false;
            rasterDesc.CullMode = D3D11_CULL_NONE;
            rasterDesc.DepthBias = 0;
            rasterDesc.DepthBiasClamp = 0.0f;
            rasterDesc.DepthClipEnable = true;
            rasterDesc.FillMode = D3D11_FILL_SOLID;
            rasterDesc.FrontCounterClockwise = false;
            rasterDesc.MultisampleEnable = false;
            rasterDesc.ScissorEnable = false;
            rasterDesc.SlopeScaledDepthBias = 0.0f;

            hr = device->CreateRasterizerState(&rasterDesc, &rasterState);

            context->RSSetState(rasterState);

            /////////////////

            D3D11_TEXTURE2D_DESC textureDesc;
            ZeroMemory(&textureDesc, sizeof(textureDesc));

            textureDesc.Width = windowWidth;
            textureDesc.Height = windowHeight;
            textureDesc.MipLevels = 1;
            textureDesc.ArraySize = 1;
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.Usage = D3D11_USAGE_STAGING;
            textureDesc.BindFlags = 0;// D3D11_BIND_RENDER_TARGET;
            textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
            textureDesc.MiscFlags = 0;

            // Create the texture
            hr = device->CreateTexture2D(&textureDesc, NULL, &renderTargetTex);

            if(hr != S_OK)
                qInfo("error creating rendertarget tex");

            ID3D11Texture2D *pRenderTarget;
            hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pRenderTarget);

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            renderTargetViewDesc.Format = textureDesc.Format;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;

            hr = device->CreateRenderTargetView(pRenderTarget, &renderTargetViewDesc, &renderTargetView);

            pRenderTarget->Release();

            if(hr != S_OK)
                qInfo("error creating rendertarget view");
        }

        return true;
    }

    void initDX(HWND hwnd, int w, int h){
        this->hWnd = hwnd;

        IDXGIFactory * DXGIFactory = nullptr;

        HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)(&DXGIFactory));

        if(hr != S_OK)
            qInfo("error creating Factory");

        DXGI_SWAP_CHAIN_DESC scd;
        scd = { 0 };
        scd.BufferCount = 1;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.OutputWindow = hWnd;
        scd.SampleDesc.Count = 1;
        scd.Windowed = TRUE;
        windowWidth = scd.BufferDesc.Width = w;
        windowHeight = scd.BufferDesc.Height = h;


        hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &scd,	&swapchain,	&device, NULL, &context);

        if(hr != S_OK){
            qInfo("error creating device or swapchain");
        }

        resizeBuffers(windowWidth, windowHeight);
    }

    void render(bool method1){
        float color[] = { 54.0f / 255.0f, 168.0f / 255.0f, 225.0f / 255.0f, 1.0f };
        context->OMSetRenderTargets( 1, &backbufferView, NULL );
        context->ClearRenderTargetView(backbufferView, color);

        if(!method1)
            swapchain->Present(1, 0); //change to (0,0) to turn off vsync, flickering will be almost gone

        QElapsedTimer  createBitmapTimer;
        createBitmapTimer.start();

        ID3D11Texture2D* pSurface;
        HRESULT hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast< void** >(&pSurface));

        if(hr != S_OK)
            return;

        context->CopyResource(renderTargetTex, pSurface);

        D3D11_MAP eMapType = D3D11_MAP_READ;
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        context->Map(renderTargetTex, 0, eMapType, NULL, &mappedResource);

        BYTE* pYourBytes = (BYTE*)mappedResource.pData;

        bitmap = QImage(pYourBytes, windowWidth, windowHeight, QImage::Format_RGBA8888);

        context->Unmap(renderTargetTex, 0);

        int elapsed_ms = createBitmapTimer.elapsed(); //This is at most 1-2ms on my i7 6700, GTX 970

        frameCounter++;

        if (fpsTimer.elapsed() >= 1000) {
            fpsTimer.restart();

            FPS = frameCounter;
            frameCounter = 0;
        }
    }

private:

    HWND						hWnd;
    int							windowWidth;
    int							windowHeight;

    IDXGISwapChain			*	swapchain;
    ID3D11Device			*	device;
    ID3D11DeviceContext		*	context;
    ID3D11RenderTargetView	*	backbufferView;

    ID3D11Texture2D			*	depthStencilBuffer;
    ID3D11DepthStencilView	*	depthStencilView;
    ID3D11RasterizerState	*	rasterState;

    ID3D11Texture2D			*	renderTargetTex;
    ID3D11RenderTargetView	*	renderTargetView;

    int                         frameCounter = 0;
    QElapsedTimer               fpsTimer;
};

#endif // DX_H

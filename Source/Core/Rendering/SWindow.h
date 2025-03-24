#pragma once
#include <Core/Math/Rect.h>

class ACamera;
class SSplitter;
struct FViewportClient;
class SWindow {
public:
	FRect Rect;

	virtual bool IsHover(FVector2D coord) const {
		return Rect.Contains(coord);
	}

	virtual void OnResizeUpdate();
	virtual void Render() {}
	virtual void OnMouseDown(FVector2D) {}
	virtual void OnMouseUp(FVector2D) {}
	virtual void OnMousePressed(FVector2D) {}
	virtual void OnMouseReleased(FVector2D) {}

	std::shared_ptr<SSplitter> child = nullptr;
	std::shared_ptr<SWindow> parent = nullptr;
	//virtual void OnDraw() = 0;
};

class SWorldWindow: public SWindow {
private:
	FViewportClient* viewportClient;
public:
	SWorldWindow(FViewportClient* viewportClient);
	~SWorldWindow();
	inline FViewportClient* GetViewportClient() { return viewportClient; }
	virtual void OnResizeUpdate() override;
	virtual void OnMousePressed(FVector2D InCoord);
};

class SSplitter : public SWindow {
protected:

	float SplitPos = 0.5f; // 분할 비율 (0.0~1.0)
	bool bIsDragging = false;

public:
	std::shared_ptr<SWindow> SideLT; // Left/Top 영역
	std::shared_ptr<SWindow> SideRB; // Right/Bottom 영역
	
	virtual void OnResize() = 0;
	virtual FRect GetSplitterRect() const = 0;

	bool IsHover(FVector2D coord) const override {
		FRect splitterRect = GetSplitterRect();
		return splitterRect.Contains(coord);
	}

	void OnMouseDown(FVector2D coord) override {
		if (IsHover(coord)) bIsDragging = true;
	}

	void OnMouseUp(FVector2D coord) override {
		bIsDragging = false;
	}
};


class SSplitterH : public SSplitter { // 가로 분할
public:
	SSplitterH(std::shared_ptr<SWindow> Left, std::shared_ptr<SWindow> Right) {
		SideLT = Left;
		SideRB = Right;
		SplitterWidth = 5.0f;
	}
	float SplitterWidth;
	void OnResize() override {
		const float splitX = Rect.Width() * SplitPos;
		SideLT->Rect = FRect(Rect.Left, Rect.Top, splitX, Rect.Height());
		SideRB->Rect = FRect(Rect.Left + splitX, Rect.Top,
			Rect.Width(), Rect.Height());
		SideLT->OnResizeUpdate();
		SideRB->OnResizeUpdate();
	}
	virtual void OnResizeUpdate() override;
private:
	FRect GetSplitterRect() const {
		return FRect(
			Rect.Left + Rect.Width() * SplitPos - SplitterWidth / 2.0f, 
			Rect.Top,
			SplitterWidth,
			Rect.Height()
		);
	}
};

class SSplitterV : public SSplitter { // 세로 분할
public:
	SSplitterV(std::shared_ptr<SWindow> Top, std::shared_ptr<SWindow> Bottom) {
		SideLT = Top;
		SideRB = Bottom;
		SplitterHeight = 5.0f;
	}
	float SplitterHeight;
	void OnResize() override {
		const float splitY = Rect.Height() * SplitPos;
		SideLT->Rect = FRect(Rect.Left, Rect.Top, Rect.Width(), splitY);
		SideRB->Rect = FRect(Rect.Left, Rect.Top + splitY,
			Rect.Width(), Rect.Height() - splitY);

		SideLT->OnResizeUpdate();
		SideRB->OnResizeUpdate();
	}
	virtual void OnResizeUpdate() override;
private:
	FRect GetSplitterRect() const {
		return FRect(
			Rect.Left, 
			Rect.Top + Rect.Height() * SplitPos - SplitterHeight / 2.0f,
			Rect.Width(),
			SplitterHeight
		);
	}
};

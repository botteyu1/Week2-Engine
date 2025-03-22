#pragma once
#include <Core/Math/Rect.h>


class SWindow {
public:
	FRect Rect;

	virtual bool IsHover(FVector2D coord) const {
		return Rect.Contains(coord);
	}

	virtual void OnResizeUpdate() {}

	//virtual void OnDraw() = 0;
};


class SSplitter : public SWindow {
protected:
	std::shared_ptr<SWindow> SideLT; // Left/Top 영역
	std::shared_ptr<SWindow> SideRB; // Right/Bottom 영역
	float SplitPos = 0.5f; // 분할 비율 (0.0~1.0)
	bool bIsDragging = false;

public:
	virtual void OnResize() = 0;
	virtual FRect GetSplitterRect() const = 0;

	bool IsHover(FVector2D coord) const override {
		FRect splitterRect = GetSplitterRect();
		return splitterRect.Contains(coord);
	}

	void OnMouseDown(FVector2D coord)  {
		if (IsHover(coord)) bIsDragging = true;
	}

	void OnMouseUp() {
		bIsDragging = false;
	}
};


class SSplitterH : public SSplitter { // 가로 분할
public:
	SSplitterH(std::shared_ptr<SWindow> Left, std::shared_ptr<SWindow> Right) {
		SideLT = Left;
		SideRB = Right;
	}

	void OnResize() override {
		const float splitX = Rect.Width() * SplitPos;
		SideLT->Rect = FRect(Rect.Left, Rect.Top, splitX, Rect.Height());
		SideRB->Rect = FRect(Rect.Left + splitX, Rect.Top,
			Rect.Width(), Rect.Height());

		SideLT->OnResizeUpdate();
		SideRB->OnResizeUpdate();

	}

private:
	FRect GetSplitterRect() const {
		return FRect(Rect.Left + Rect.Width() * SplitPos - 2, Rect.Top,
			4, Rect.Height());
	}
};

class SSplitterV : public SSplitter { // 세로 분할
public:
	SSplitterV(std::shared_ptr<SWindow> Top, std::shared_ptr<SWindow> Bottom) {
		SideLT = Top;
		SideRB = Bottom;
	}

	void OnResize() override {
		const float splitY = Rect.Height() * SplitPos;
		SideLT->Rect = FRect(Rect.Left, Rect.Top, Rect.Width(), splitY);
		SideRB->Rect = FRect(Rect.Left, Rect.Top + splitY,
			Rect.Width(), Rect.Height() - splitY);

		SideLT->OnResizeUpdate();
		SideRB->OnResizeUpdate();
	}

private:
	FRect GetSplitterRect() const {
		return FRect(Rect.Left, Rect.Top + Rect.Height() * SplitPos - 2,
			Rect.Width(), 4);
	}
};

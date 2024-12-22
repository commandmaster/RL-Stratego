#include "raylib.h"
#include "core.h"

#include <string>
#include <cmath>
#include <algorithm>
#include <functional>
#include <vector>

namespace UI 
{

enum class Anchor 
{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    Center,
    TopCenter,
    BottomCenter,
    CenterLeft,
    CenterRight
};


class UIElement
{
public:
    static float CalculateFontSize(Font font, float desiredHeight) {
		Vector2 baseSize = MeasureTextEx(font, "A", 1.0f, 1.0f);
		float baseHeight = baseSize.y;

		return desiredHeight / baseHeight;
	}

    UIElement(float x, float y, Anchor anchor, bool originCenter)
        : x(x), y(y), anchor(anchor), originCenter(originCenter)
    { }

    virtual void Draw() {}

	void SetPosition(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    void AddPosition(float x, float y)
    {
        this->x += x;
        this->y += y;
    }

	void SetBaseResolution(float baseWidth, float baseHeight) 
    {
        this->baseWidth = baseWidth;
        this->baseHeight = baseHeight;
    }

    void SetAnchor(Anchor anchor)
    {
        this->anchor = anchor;
    }

protected:
    float x, y;
    float baseWidth = 1920.0f, baseHeight = 1080.0f;
    Anchor anchor;
    bool originCenter;

    Vector2 CalculatePosition(float textWidth, float textHeight) const
    {
        float screenWidth = static_cast<float>(GetScreenWidth());
        float screenHeight = static_cast<float>(GetScreenHeight());

        Vector2 anchorPoint;
        switch (anchor) {
        case Anchor::TopLeft:
            anchorPoint = { 0.0f, 0.0f };
            break;
        case Anchor::TopRight:
            anchorPoint = { screenWidth, 0.0f };
            break;
        case Anchor::BottomLeft:
            anchorPoint = { 0.0f, screenHeight };
            break;
        case Anchor::BottomRight:
            anchorPoint = { screenWidth, screenHeight };
            break;
        case Anchor::Center:
            anchorPoint = { screenWidth / 2, screenHeight / 2 };
            break;
        case Anchor::TopCenter:
            anchorPoint = { screenWidth / 2, 0.0f };
            break;
        case Anchor::BottomCenter:
            anchorPoint = { screenWidth / 2, screenHeight };
            break;
        case Anchor::CenterLeft:
            anchorPoint = { 0.0f, screenHeight / 2 };
            break;
        case Anchor::CenterRight:
            anchorPoint = { screenWidth, screenHeight / 2 };
            break;
        }

        // Scale position offset relative to base resolution
        Vector2 scaledOffset = {
            x * screenWidth / baseWidth,
            y * screenHeight / baseHeight
        };

        Vector2 position = { anchorPoint.x + scaledOffset.x, anchorPoint.y + scaledOffset.y };

        if (originCenter) 
        {
            position.x -= textWidth / 2;
            position.y -= textHeight / 2;
        }

        return position;
    }

};

class Label : public UIElement {
public:
    Label(float x, float y, const std::string& text, Anchor anchor = Anchor::TopLeft, bool originCenter = false, int fontSize = 20)
        : UIElement(x, y, anchor, originCenter), text(text), fontSize(fontSize) {}

    void Draw() 
    {
        float screenWidth = static_cast<float>(GetScreenWidth());
        float screenHeight = static_cast<float>(GetScreenHeight());

        // Scale font size dynamically
        int scaledFontSize = static_cast<int>(fontSize * screenHeight / baseHeight);

        // Measure text dimensions
        int textWidth = MeasureText(text.c_str(), scaledFontSize);
        int textHeight = scaledFontSize;

        Vector2 position = CalculatePosition(static_cast<float>(textWidth), static_cast<float>(textHeight));

        // Draw the text
        DrawText(text.c_str(), static_cast<int>(position.x), static_cast<int>(position.y), scaledFontSize, textColor);
    }

    void SetText(const std::string& newText) 
    {
        text = newText;
    }

    void SetTextColor(Color color) 
    {
        textColor = color;
    }

    

private:
    int fontSize;

    std::string text = "Label";
    Color textColor = WHITE;

};


class Button : public UIElement{
public:
    Button(float x, float y, float width, float height, Anchor anchor = Anchor::TopLeft, bool originCenter = false, int fontSize = 20)
        : UIElement(x, y, anchor, originCenter), width(width), height(height), fontSize(fontSize) {}

	void Draw()
    {
        float scaledWidth = width * GetScreenWidth() / baseWidth;
        float scaledHeight = height * GetScreenHeight() / baseHeight;

        Vector2 position = CalculatePosition(scaledWidth, scaledHeight);
        Rectangle rect = { position.x, position.y, scaledWidth, scaledHeight };

        // Draw button rectangle
        DrawRectangleRec(rect, buttonColor);

        // Scale font size dynamically
        int scaledFontSize = static_cast<int>(fontSize * GetScreenHeight() / baseHeight);

        // Measure text dimensions
        int textWidth = MeasureText(label.c_str(), scaledFontSize);
        int textHeight = scaledFontSize;

        // Center text inside the button
        float textX = rect.x + (rect.width - textWidth) / 2;
        float textY = rect.y + (rect.height - textHeight) / 2;

        // Draw button label
        DrawText(label.c_str(), static_cast<int>(textX), static_cast<int>(textY), scaledFontSize, labelColor);
    }

    bool IsClicked() 
    {
        return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && IsHovered();
    }

    bool IsHovered()
    {
        float scaledWidth = width * GetScreenWidth() / baseWidth;
        float scaledHeight = height * GetScreenHeight() / baseHeight;

        Vector2 position = CalculatePosition(scaledWidth, scaledHeight);
        Rectangle rect = { position.x, position.y, scaledWidth, scaledHeight };

        return CheckCollisionPointRec(GetMousePosition(), rect);
    }

    void SetLabel(const std::string& text)
    {
        label = text;
    }

    void SetButtonColor(Color color)
    {
        buttonColor = color;
    }

    void SetLabelColor(Color color)
    {
        labelColor = color;
    }

private:
	int fontSize;
    float width, height;

    std::string label = "Button";
    Color buttonColor = DARKGRAY;
    Color labelColor = WHITE;

};

class InputBox : public UIElement
{
public:
    InputBox(float x, float y, float width, float height, Anchor anchor = Anchor::TopLeft, bool originCenter = false)
        : UIElement(x, y, anchor, originCenter), width(width), height(height)
    {}

    void OnSubmit(const std::function<void(InputBox&)>& callback)
    {
        callbacks.emplace_back(std::move(callback));
    }

    void Draw()
    {
        float scaledWidth = width * GetScreenWidth() / baseWidth;
        float scaledHeight = height * GetScreenHeight() / baseHeight;

        Vector2 position = CalculatePosition(scaledWidth, scaledHeight);
        Rectangle rect = { position.x, position.y, scaledWidth, scaledHeight };

        int fontSize = (int)UIElement::CalculateFontSize(GetFontDefault(), height - height / 4);
        int scaledFontSize = static_cast<int>(fontSize * GetScreenHeight() / baseHeight);
        int textWidth = MeasureText(text.c_str(), scaledFontSize);
        int textHeight = scaledFontSize;

        rect.width = std::max<float>(scaledWidth, static_cast<float>(textWidth) + static_cast<float>(textWidth / 5));

        DrawRectangleRec(rect, backgroundColor);
        DrawRectangleLinesEx(rect, borderThickness * std::min<float>(GetScreenWidth() / baseWidth, GetScreenHeight() / baseHeight), borderColor);


        float textX = rect.x + (rect.width - textWidth) / 2;
        float textY = rect.y + (rect.height - textHeight) / 2;

        DrawText(text.c_str(), static_cast<int>(textX), static_cast<int>(textY), scaledFontSize, labelTextColor); // Label

        blinkCounter += GetFrameTime();
        if (!blink) blinkCounter = 0.0f;

        if ((blinkCounter < 0.53f || !blink) && focused)
        {
			DrawRectangle(textX + MeasureText(std::string(text.begin(), text.begin() + cursorPos).c_str(), scaledFontSize), textY + (float)textHeight / 16, 4, textHeight - (float)textHeight / 8, BLACK); // Cursor
        }
        else if (blinkCounter > 1.06f)
        {
            blinkCounter = 0.0f;
        }
    }

    void CheckForInput()
    {
		if (IsKeyPressed(KEY_ENTER) && focused)
        {
            for (auto& callback : callbacks)
            {
                callback(*this);
            }
        }

        if (CheckCollisionPointRec(GetMousePosition(), calculateBounds()) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            focused = true;
        }
        else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER))
        {
            focused = false;
        }

        if (!focused) return;

        int key = GetCharPressed();
        blink = (GetKeyPressed() == 0);

        constexpr int maxChars = 1000;
        while (key > 0) 
        { 
            if ((key >= 32 && key <= 126) && text.size() < maxChars) 
            {         
                text.insert(text.begin() + cursorPos, static_cast<char>(key));
                cursorPos++; // will be clamped at the end of the loop
            }             
            
            key = GetCharPressed(); 
        }
        
        if ((IsKeyPressedRepeat(KEY_BACKSPACE) || IsKeyPressed(KEY_BACKSPACE)) && !text.empty() && cursorPos >= 1) text.erase(text.begin() + cursorPos-- - 1);
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) cursorPos++;
        else if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) cursorPos--;
        cursorPos = std::clamp<int>(cursorPos, 0, text.size());
        
    }

    void SetLabelText(const std::string& text)
    {
        this->text = text;
    }

    void SetLabelTextColor(Color color)
    {
        labelTextColor = color;
    }

    void SetBackgroundColor(Color color)
    {
        backgroundColor = color;
    }

    void SetBorderColor(Color color)
    {
        borderColor = color;
    }

    void SetBorderThickness(int thickness)
    {
        borderThickness = thickness;
    }

    std::string GetLabelText() const
    {
        return text;
    }


private:
    Rectangle calculateBounds()
    {
        float scaledWidth = width * GetScreenWidth() / baseWidth;
        float scaledHeight = height * GetScreenHeight() / baseHeight;

        Vector2 position = CalculatePosition(scaledWidth, scaledHeight);
        Rectangle rect = { position.x, position.y, scaledWidth, scaledHeight };

        int fontSize = (int)UIElement::CalculateFontSize(GetFontDefault(), height - height / 4);
        int scaledFontSize = static_cast<int>(fontSize * GetScreenHeight() / baseHeight);
        int textWidth = MeasureText(text.c_str(), scaledFontSize);
        int textHeight = scaledFontSize;

        rect.width = std::max<float>(scaledWidth, static_cast<float>(textWidth) + static_cast<float>(textWidth / 5));

        return rect;
    }

    float width, height;
    int borderThickness = 2;
    std::string text = "";
    Color labelTextColor = WHITE;
    Color backgroundColor = DARKGRAY;
    Color borderColor = BLACK;
    
    // cursor
    int cursorPos;
    bool focused = false;
    bool blink = false;
    float blinkCounter = 0.0f;

    // callbacks for On Enter Event
    std::vector<std::function<void(InputBox&)>> callbacks = {};

};

} // namespace UI

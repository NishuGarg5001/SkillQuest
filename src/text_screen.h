#ifndef TEXT_SCREEN_H
#define TEXT_SCREEN_H

#include "screen.h"

struct Word
{
    const std::string text;
    const SDL_Color color;
    const float pos_x;
    float pos_y;
    const size_t width;

    explicit Word(std::string text, SDL_Color color, float x, float y, size_t width) :
    text(std::move(text)),
    color(color),
    pos_x(x),
    pos_y(y),
    width(width)
    {}
};

class TextScreen : public Screen
{
    std::deque<std::vector<Word>> text_buffer;

    public:
        TextScreen(float x, float y, float w, float h) : Screen(x, y, w, h)
        {}

        void clearTextBuffer() noexcept
        {
            text_buffer.clear();
        }

        size_t getTextLen(const std::string& text, TTF_Font *font) const noexcept
        {
            int w, h;
            w = h = 0;
            TTF_GetStringSize(font, text.c_str(), text.size(), &w, &h);
            return static_cast<size_t>(w);
        }

        void shiftTextUp()
        {
            for (auto& line : text_buffer)
                for (auto& word : line)
                    word.pos_y -= FONT_SIZE;
        }

        void makeSpaceTextBuffer()
        {
            if(text_buffer.size() == NUM_LINES)
                text_buffer.pop_front();
            shiftTextUp();
        }

        void pushTextToTextBuffer(const std::vector<std::string>& words, const std::vector<SDL_Color>& colors, TTF_Font *font)
        {
            makeSpaceTextBuffer();
            text_buffer.emplace_back();
            const size_t space_size = getTextLen(" ", font);
            size_t buffer_counter = 0;
            for (size_t i = 0; i < words.size(); i++)
            {
                size_t w = getTextLen(words[i], font);
                if(buffer_counter + w <= static_cast<size_t>(getWidth()))
                {
                    float x = static_cast<float>(buffer_counter);
                    float y = getY() + getHeight() - FONT_SIZE;
                    text_buffer.back().emplace_back(words[i], colors[i], x, y, w);
                    buffer_counter += w + space_size;
                }
                else
                {
                    makeSpaceTextBuffer();
                    text_buffer.emplace_back();
                    buffer_counter = 0;
                    i--;
                }
            }
        }

        void drawText(const std::string& text, const float& x, const float& y, size_t w, SDL_Renderer *renderer,
            TTF_Font *font, const SDL_Color& color = WHITE, const float& h = FONT_SIZE) const
        {
            SDL_Surface* text_surface = TTF_RenderText_Blended(font, text.c_str(), text.size(), color);
            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            SDL_DestroySurface(text_surface);
            SDL_FRect dst {x, y, static_cast<float>(w), h};
            SDL_RenderTexture(renderer, text_texture, nullptr, &dst);
            SDL_DestroyTexture(text_texture);
        }

        void render(SDL_Renderer *renderer, TTF_Font *font) const
        {
            renderBox(renderer);
            for(const std::vector<Word>& word_line : text_buffer)
                for(const Word& word : word_line)
                    drawText(word.text, word.pos_x, word.pos_y, word.width, renderer, font, word.color);
        }

        void startedMining(const std::string& res_name, TTF_Font *font)
        {
            pushTextToTextBuffer({"You", "started", "to", "mine", res_name+"."}, {WHITE, WHITE, WHITE, WHITE, WHITE}, font);
        }

        void mineSuccess(const std::string& obj_name, SDL_Color rarity_color, TTF_Font *font)
        {
            pushTextToTextBuffer({"You", "mined", "a", obj_name+"."}, {WHITE, WHITE, WHITE, rarity_color}, font);
        }

        void inventoryFull(TTF_Font *font)
        {
            pushTextToTextBuffer({"Your", "inventory", "is", "full!"}, {WHITE, WHITE, WHITE, WHITE}, font);
        }
};

#endif
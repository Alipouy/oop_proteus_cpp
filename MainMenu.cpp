#include "MainMenu.h"
#include <fstream>
#include <iostream>

MainMenu::MainMenu() {
    loadRecentProjects();
}

void MainMenu::loadRecentProjects() {
    // نمونه‌سازی چند پروژه اخیر برای نمایش در منو (می‌توانید بعداً به فایل متصل کنید)
    recentProjects = {
            {"Project_Filter_Test", "projects/filter.ckt", "2026-08-16 14:30"},
            {"Logic_Gate_Adder", "projects/adder.ckt", "2026-08-15 09:20"},
            {"OpAmp_Signal_Conditioner", "projects/opamp.ckt", "2026-08-12 18:45"}
    };
}

void MainMenu::renderButton(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_FRect rect, bool hovered) {
    if (hovered) {
        SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255); // رنگ هاور جذاب
    } else {
        SDL_SetRenderDrawColor(renderer, 45, 52, 54, 255); // رنگ پس‌زمینه دکمه
    }
    SDL_RenderFillRectF(renderer, &rect);

    // کادر دور دکمه
    SDL_SetRenderDrawColor(renderer, 116, 125, 140, 255);
    SDL_RenderDrawRectF(renderer, &rect);

    if (font && !text.empty()) {
        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), textColor);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            if (tex) {
                SDL_FRect dest = { rect.x + (rect.w - surf->w) / 2.0f, rect.y + (rect.h - surf->h) / 2.0f, (float)surf->w, (float)surf->h };
                SDL_RenderCopyF(renderer, tex, nullptr, &dest);
                SDL_DestroyTexture(tex);
            }
            SDL_FreeSurface(surf);
        }
    }
}

MenuResult MainMenu::handleEvent(const SDL_Event& event, int windowWidth, int windowHeight) {
    float mx = 0, my = 0;
    if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
        mx = (float)event.motion.x;
        my = (float)event.motion.y;
    }

    float centerX = windowWidth / 2.0f - 150.0f;
    float startY = windowHeight / 2.0f - 120.0f;

    // اگر پنجره تنظیمات بوم باز است، رویدادها را آنجا مدیریت کن
    if (showCanvasConfigModal) {
        SDL_FRect okBtn = {windowWidth / 2.0f - 110.0f, windowHeight / 2.0f + 90.0f, 100.0f, 40.0f};
        SDL_FRect cancelBtn = {windowWidth / 2.0f + 10.0f, windowHeight / 2.0f + 90.0f, 100.0f, 40.0f};

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (mx >= okBtn.x && mx <= okBtn.x + okBtn.w && my >= okBtn.y && my <= okBtn.y + okBtn.h) {
                showCanvasConfigModal = false;
                return MenuResult::NEW_PROJECT;
            }
            if (mx >= cancelBtn.x && mx <= cancelBtn.x + cancelBtn.w && my >= cancelBtn.y && my <= cancelBtn.y + cancelBtn.h) {
                showCanvasConfigModal = false;
            }
        }
        return MenuResult::NONE;
    }

    // دکمه‌های منوی اصلی
    SDL_FRect btnNew = {centerX, startY, 300.0f, 50.0f};
    SDL_FRect btnOpen = {centerX, startY + 70.0f, 300.0f, 50.0f};
    SDL_FRect btnExit = {centerX, startY + 140.0f, 300.0f, 50.0f};

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (mx >= btnNew.x && mx <= btnNew.x + btnNew.w && my >= btnNew.y && my <= btnNew.y + btnNew.h) {
            showCanvasConfigModal = true; // باز کردن پنجره انتخاب ابعاد بوم
        }
        else if (mx >= btnOpen.x && mx <= btnOpen.x + btnOpen.w && my >= btnOpen.y && my <= btnOpen.y + btnOpen.h) {
            return MenuResult::OPEN_PROJECT; // باز کردن فایل از سیستم
        }
        else if (mx >= btnExit.x && mx <= btnExit.x + btnExit.w && my >= btnExit.y && my <= btnExit.y + btnExit.h) {
            return MenuResult::EXIT;
        }

        // بررسی کلیک روی پروژه‌های اخیر
        float recentStartY = startY + 210.0f;
        for (size_t i = 0; i < recentProjects.size(); ++i) {
            SDL_FRect rItem = {centerX, recentStartY + (i * 35.0f), 300.0f, 30.0f};
            if (mx >= rItem.x && mx <= rItem.x + rItem.w && my >= rItem.y && my <= rItem.y + rItem.h) {
                selectedRecentPath = recentProjects[i].filePath;
                return MenuResult::OPEN_RECENT;
            }
        }
    }

    return MenuResult::NONE;
}

void MainMenu::render(SDL_Renderer* renderer, TTF_Font* font, int windowWidth, int windowHeight) {
    // پس‌زمینه تیره و مدرن منو
    SDL_SetRenderDrawColor(renderer, 30, 39, 46, 255);
    SDL_RenderClear(renderer);

    float centerX = windowWidth / 2.0f - 150.0f;
    float startY = windowHeight / 2.0f - 120.0f;

    // عنوان برنامه
    if (font) {
        SDL_Color titleColor = {255, 168, 1, 255};
        SDL_Surface* titleSurf = TTF_RenderText_Blended(font, "CIRCUIT SIMULATOR - MAIN MENU", titleColor);
        if (titleSurf) {
            SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer, titleSurf);
            SDL_FRect titleRect = {windowWidth / 2.0f - titleSurf->w / 2.0f, startY - 70.0f, (float)titleSurf->w, (float)titleSurf->h};
            SDL_RenderCopyF(renderer, titleTex, nullptr, &titleRect);
            SDL_DestroyTexture(titleTex);
            SDL_FreeSurface(titleSurf);
        }
    }

    // دکمه‌های اصلی
    renderButton(renderer, font, "Create New Project", {centerX, startY, 300.0f, 50.0f}, false);
    renderButton(renderer, font, "Open Project from File", {centerX, startY + 70.0f, 300.0f, 50.0f}, false);
    renderButton(renderer, font, "Exit", {centerX, startY + 140.0f, 300.0f, 50.0f}, false);

    // بخش فهرست پروژه‌های اخیر (Recent Projects)
    if (font && !recentProjects.empty()) {
        SDL_Color labelColor = {178, 190, 195, 255};
        SDL_Surface* lblSurf = TTF_RenderText_Blended(font, "Recent Projects:", labelColor);
        if (lblSurf) {
            SDL_Texture* lblTex = SDL_CreateTextureFromSurface(renderer, lblSurf);
            SDL_FRect lblRect = {centerX, startY + 205.0f, (float)lblSurf->w, (float)lblSurf->h};
            SDL_RenderCopyF(renderer, lblTex, nullptr, &lblRect);
            SDL_DestroyTexture(lblTex);
            SDL_FreeSurface(lblSurf);
        }

        float recentStartY = startY + 235.0f;
        for (size_t i = 0; i < recentProjects.size(); ++i) {
            SDL_FRect rItem = {centerX, recentStartY + (i * 35.0f), 300.0f, 30.0f};
            SDL_SetRenderDrawColor(renderer, 47, 54, 64, 255);
            SDL_RenderFillRectF(renderer, &rItem);

            std::string displayText = recentProjects[i].name + " (" + recentProjects[i].lastModified + ")";
            SDL_Surface* itemSurf = TTF_RenderText_Blended(font, displayText.c_str(), {220, 221, 225, 255});
            if (itemSurf) {
                SDL_Texture* itemTex = SDL_CreateTextureFromSurface(renderer, itemSurf);
                SDL_FRect itemRect = {rItem.x + 10.0f, rItem.y + 5.0f, (float)(itemSurf->w > 280 ? 280 : itemSurf->w), (float)itemSurf->h};
                SDL_RenderCopyF(renderer, itemTex, nullptr, &itemRect);
                SDL_DestroyTexture(itemTex);
                SDL_FreeSurface(itemSurf);
            }
        }
    }

    // پنجره پاپ‌آپ تنظیمات بوم (هنگام کلیک روی ایجاد پروژه جدید)
    if (showCanvasConfigModal) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180); // پس‌زمینه نیمه‌شفاف
        SDL_FRect overlay = {0, 0, (float)windowWidth, (float)windowHeight};
        SDL_RenderFillRectF(renderer, &overlay);

        SDL_FRect modalRect = {windowWidth / 2.0f - 200.0f, windowHeight / 2.0f - 150.0f, 400.0f, 300.0f};
        SDL_SetRenderDrawColor(renderer, 47, 54, 64, 255);
        SDL_RenderFillRectF(renderer, &modalRect);
        SDL_SetRenderDrawColor(renderer, 241, 196, 15, 255);
        SDL_RenderDrawRectF(renderer, &modalRect);

        if (font) {
            SDL_Surface* mSurf = TTF_RenderText_Blended(font, "New Project Canvas Settings", {255, 255, 255, 255});
            if (mSurf) {
                SDL_Texture* mTex = SDL_CreateTextureFromSurface(renderer, mSurf);
                SDL_FRect mRect = {modalRect.x + 20.0f, modalRect.y + 20.0f, (float)mSurf->w, (float)mSurf->h};
                SDL_RenderCopyF(renderer, mTex, nullptr, &mRect);
                SDL_DestroyTexture(mTex);
                SDL_FreeSurface(mSurf);
            }

            // گزینه‌های پیش‌تنظیم ابعاد (A4, A3, HD)
            SDL_Surface* pSurf = TTF_RenderText_Blended(font, "Presets: A4 (Default) / Custom 1920x1080", {150, 206, 180, 255});
            if (pSurf) {
                SDL_Texture* pTex = SDL_CreateTextureFromSurface(renderer, pSurf);
                SDL_FRect pRect = {modalRect.x + 20.0f, modalRect.y + 80.0f, (float)pSurf->w, (float)pSurf->h};
                SDL_RenderCopyF(renderer, pTex, nullptr, &pRect);
                SDL_DestroyTexture(pTex);
                SDL_FreeSurface(pSurf);
            }
        }

        // دکمه‌های تایید و انصراف در مودال
        renderButton(renderer, font, "Create", {windowWidth / 2.0f - 110.0f, windowHeight / 2.0f + 90.0f, 100.0f, 40.0f}, false);
        renderButton(renderer, font, "Cancel", {windowWidth / 2.0f + 10.0f, windowHeight / 2.0f + 90.0f, 100.0f, 40.0f}, false);
    }
}
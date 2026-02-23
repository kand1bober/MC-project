
#include "interrupts.h"
#include "game.h"
#include "basics.h"

const unsigned char PROGMEM storm [] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7F, 0xFE, 0x00, 0x00, 0x00, 0x07, 0x80, 0x01, 0xE0, 0x00, 0x00, 0x0C,
    0x00, 0x00, 0x20, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00, 0x00, 0x30, 0x00, 0x00, 0x04, 0x00,
    0x00, 0x20, 0x00, 0x00, 0x04, 0x00, 0x00, 0x20, 0x00, 0x00, 0x04, 0x00, 0x00, 0x60, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x40, 0x00, 0x00, 0x02, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 0x40,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 0x7F, 0xE0, 0x00, 0x01, 0x00,
    0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xD7, 0xFF, 0xFF,
    0xE1, 0x00, 0x01, 0xBF, 0xFC, 0x1F, 0xFA, 0x80, 0x01, 0xBF, 0xF1, 0xCF, 0xFA, 0x80, 0x01, 0x3F,
    0xC2, 0x37, 0xF7, 0x80, 0x01, 0xEF, 0x9C, 0x01, 0xE7, 0xC0, 0x01, 0xE0, 0x70, 0x06, 0x06, 0x80,
    0x01, 0xE0, 0xC0, 0x03, 0x06, 0x80, 0x01, 0xFF, 0x80, 0x01, 0xFF, 0x80, 0x01, 0xF8, 0x00, 0x00,
    0x1D, 0xC0, 0x03, 0x70, 0x00, 0x80, 0x0C, 0x60, 0x05, 0xB0, 0x07, 0xF0, 0x08, 0x90, 0x09, 0x10,
    0x1F, 0xF8, 0x09, 0xD0, 0x0B, 0x90, 0x1F, 0x7C, 0x03, 0xF0, 0x0F, 0xC0, 0xFC, 0x0F, 0x07, 0x90,
    0x0D, 0x43, 0xC0, 0x03, 0x07, 0x90, 0x05, 0x64, 0x00, 0x00, 0xCF, 0x10, 0x07, 0xFC, 0x00, 0x00,
    0x26, 0x10, 0x01, 0x80, 0x00, 0x00, 0x10, 0x20, 0x01, 0x00, 0x00, 0x00, 0x0E, 0x40, 0x01, 0x80,
    0x07, 0xF0, 0x01, 0x80, 0x00, 0x80, 0x07, 0xC8, 0x00, 0x80, 0x00, 0x80, 0x0B, 0xE8, 0x00, 0x80,
    0x00, 0x87, 0x97, 0xE9, 0xE0, 0x80, 0x00, 0x87, 0xDF, 0xEF, 0xA0, 0x80, 0x00, 0x4B, 0xFF, 0xFF,
    0xA0, 0x80, 0x00, 0x6B, 0xDF, 0xFB, 0xA3, 0x00, 0x00, 0x24, 0x97, 0xE8, 0x24, 0x00, 0x00, 0x1E,
    0x1F, 0xC0, 0x2C, 0x00, 0x00, 0x07, 0xF8, 0x1F, 0xF0, 0x00, 0x00, 0x00, 0x0F, 0xF8, 0x00, 0x00
};

// flags for display
volatile bool to_display = false;
volatile bool display_needs_update = false;

// flags for buttons(like on dualshock)
volatile bool button_up = false; 
volatile bool button_down = false; 
volatile bool button_left = false;
volatile bool button_right = false;
volatile bool button_cross = false;

void game_t::read_buttons() {
    uint8_t oldSREG = SREG;
    cli();

    // crit section
    if (button_up) { buttons_vector_.up = true; }
    if (button_down) { buttons_vector_.down = true; }
    if (button_left) { buttons_vector_.left = true; }
    if (button_right) { buttons_vector_.right = true; }
    if (button_cross) { buttons_vector_.cross = true; }

    SREG = oldSREG;
}

void game_t::draw_entity(entity_t& obj) {
    monitor_.drawXBMP(obj.x_, obj.y_,
                      obj.bitmap.w_, obj.bitmap.h_,
                      obj.bitmap.bits_);
}

void game_t::draw_bitmap(uint8_t x, uint8_t y, bitmap_t& bitmap) {
    monitor_.drawXBMP(x, y, 
                      bitmap.w_, bitmap.h_, 
                      bitmap.bits_);
}

void game_t::shoot(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t vx, uint8_t vy) {
    for (auto &b : bullets_) {
        if (!b.active_) {
            b.x_ = x;
            b.y_ = y;
            b.w_ = w;
            b.h_ = h;
            b.vx_ = vx;
            b.vy_ = vy;
            b.active_ = true;
            return;
        }
    }
}

// update current bullet collisions with game units
void game_t::update_collisions(bullet_t& bullet) {
    if (bullet.x_ + bullet.w_ >= enemy_.x_ && 
        bullet.x_ + bullet.w_ <= enemy_.x_ + enemy_.w_ && 
        bullet.y_ >= enemy_.y_ &&
        bullet.y_ <= enemy_.y_ + enemy_.h_) {
        
        // draw explosion & remove bullet with enemy
        bitmap_t explosion(explosion_r7, 15, 15);
        draw_bitmap(bullet.x_ - 7, bullet.y_ - 7, explosion);
        bullet.active_ = false;
    }
}

void game_t::update_bullets() {
    for (auto &b : bullets_) {
        if (b.active_) {
            if (b.x_ <= 128 && b.y_ <= 64) {
                monitor_.drawBox(b.x_, b.y_, b.w_, b.h_);
                b.x_ += b.vx_ * 1;
                // b.y = += b.vy_ * 1; 

                update_collisions(b);
            }
            else {
                b.active_ = false;
            }
        }
    }
}

// make state for current frame of the game
void game_t::update_state() {
    monitor_.clearBuffer(); // clear full frame
    
    if (game_state_ == kHlt) {
        draw_grid(monitor_);
        monitor_.setFont(u8g2_font_6x10_tf);
        monitor_.drawStr(5, 12, "Space kill");

        if (buttons_vector_.cross) {
            buttons_vector_.cross = false;
            button_cross = false;
            game_state_ = kRunning;
        }
        
        monitor_.sendBuffer(); // draw full frame
        return;
    }

    // draw stars
    for (size_t i = 0; i < sizeof(stars)/sizeof(stars[0]); i++) {
        monitor_.drawPixel(stars[i][0], stars[i][1]);
    }

    // set player position
    if (buttons_vector_.up) {
        buttons_vector_.up = false;
        player_.y_ -= 1;
    }
    if (buttons_vector_.down) {
        buttons_vector_.down = false;
        player_.y_ += 1;
    }
    if (buttons_vector_.left) {
        buttons_vector_.left = false;
        player_.x_ -= 1;
    }
    if (buttons_vector_.right) {
        buttons_vector_.right = false;
        player_.x_ += 1;
    }
    if (buttons_vector_.cross) {
        buttons_vector_.cross = false;
        button_cross = false;

        shoot(player_.x_, player_.y_, 3, 1, 10, 0);
    }
    draw_entity(player_);
    draw_entity(enemy_);    

    update_bullets();

    monitor_.sendBuffer(); // draw full frame
}

int main() {
    init_controller();
    init_pins_and_interrupts();
    
    game_t game{};

    while(1) {
        game.read_buttons();
        game.update_state();
    }
}

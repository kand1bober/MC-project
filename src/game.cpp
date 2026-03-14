#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "interrupts.h"
#include "game.h"
#include "basics.h"

// flags for display
volatile bool to_display = false;
volatile bool display_needs_update = false;

// flags for buttons(like on dualshock)
volatile bool button_up = false;
volatile bool button_down = false;
volatile bool button_left = false;
volatile bool button_right = false;
volatile bool button_cross = false;

void game_t::read_buttons()
{
    uint8_t oldSREG = SREG;
    cli();

    // crit section
    if (button_up)
    {
        buttons_vector_.up = true;
    }
    if (button_down)
    {
        buttons_vector_.down = true;
    }
    if (button_left)
    {
        buttons_vector_.left = true;
    }
    if (button_right)
    {
        buttons_vector_.right = true;
    }
    if (button_cross)
    {
        buttons_vector_.cross = true;
    }

    SREG = oldSREG;
}

void game_t::draw_entity(entity_t &obj)
{
    monitor_.drawXBMP(obj.x_, obj.y_,
                      obj.bitmap.w_, obj.bitmap.h_,
                      obj.bitmap.bits_);
}

void game_t::draw_bitmap(uint8_t x, uint8_t y, bitmap_t &bitmap)
{
    monitor_.drawXBMP(x, y,
                      bitmap.w_, bitmap.h_,
                      bitmap.bits_);
}

void game_t::shoot(uint8_t x, uint8_t y, uint8_t w, uint8_t h, int8_t vx, int8_t vy)
{
    for (auto &b : bullets_)
    {
        if (!b.active_)
        {
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
void game_t::update_collisions(bullet_t &bullet)
{
    if (bullet.x_ + bullet.w_ >= enemy_.x_ &&
        bullet.x_ + bullet.w_ <= enemy_.x_ + enemy_.w_ &&
        bullet.y_ >= enemy_.y_ &&
        bullet.y_ <= enemy_.y_ + enemy_.h_)
    {

        // draw explosion
        bitmap_t explosion(explosion_r7, 15, 15);
        draw_bitmap(enemy_.x_ - 3,
                    enemy_.y_ - 3,
                    explosion);

        // remove bullet
        bullet.active_ = false;

        if (--game_t::enemy_.hp_ <= 0)
        {
            // TODO: Animation of winning
            game_t::game_state_ = game_state_t::kHlt;
        }
    }
}

void game_t::update_bullets()
{
    for (auto &b : bullets_)
    {
        if (b.active_)
        {
            if (b.x_ <= 128 && b.x_ >= 0 && b.y_ >= 0 && b.y_ <= 64)
            {
                monitor_.drawBox(b.x_, b.y_, b.w_, b.h_);
                b.x_ += b.vx_ * 1;
                // b.y = += b.vy_ * 1;

                update_collisions(b);
            }
            else
            {
                b.active_ = false;
            }
        }
    }
}

// update kHlt / kRunning / kPause
void game_t::update_pause_state()
{
    // Draw game icon and invitation to play
    draw_invitation(monitor_);

    if (buttons_vector_.cross)
    {
        buttons_vector_.cross = false;
        button_cross = false;
        game_state_ = kRunning;

        init_game_state();
    }

    monitor_.sendBuffer(); // draw full frame
    return;
}

void game_t::update_player_pos()
{
    #define kSpeed 2
    if (buttons_vector_.up)
    {
        buttons_vector_.up = false;
        player_.y_ -= kSpeed;
    }
    if (buttons_vector_.down)
    {
        buttons_vector_.down = false;
        player_.y_ += kSpeed;
    }
    if (buttons_vector_.left)
    {
        buttons_vector_.left = false;
        player_.x_ -= kSpeed;
    }
    if (buttons_vector_.right)
    {
        buttons_vector_.right = false;
        player_.x_ += kSpeed;
    }
    if (buttons_vector_.cross)
    {
        buttons_vector_.cross = false;
        button_cross = false;

        shoot(player_.x_ + player_.bitmap.w_,
              player_.y_ + player_.bitmap.h_ / 2,
              3, 1,
              10, 0);
    }
}

void game_t::update_enemy_state(entity_t &enemy)
{
    #define kDiff 2

    // update of position
    if (enemy.y_ == enemy.y_target_ ||
        (enemy.y_ > enemy.y_target_ && enemy.vy_ > 0) ||
        (enemy.y_ < enemy.y_target_ && enemy.vy_ < 0)
    )
    {
        uint8_t next_y_target;
        do
        {
            next_y_target = rand() % (game_t::display_h - enemy.bitmap.h_ + 1);
        } while (next_y_target == enemy.y_target_);

        enemy.y_target_ = next_y_target;

        if (enemy.y_ > enemy.y_target_) {
            enemy.vy_ = -2  ;
        }
        else {
            enemy.vy_ = 2;
        }
    }
    else {
        enemy.y_ += enemy.vy_;
    }

    // show velocity
    game_t::monitor_.setFont(u8g2_font_tenthinnerguys_tr);
    char str[8];
    sprintf(str, "V_y= %d", enemy_.vy_);
    game_t::monitor_.drawStr(15, 60, str);

    // update shooting
    if (abs(enemy.y_shoot_target_ - enemy.y_ ) < kDiff)
    {
        shoot(enemy_.x_,
              enemy_.y_ + enemy_.bitmap.h_ / 2,
              3, 1,
              -10, 0);

        uint8_t next_y_shoot_target;
        do
        {
            next_y_shoot_target = rand() % (game_t::display_h - enemy.bitmap.h_ + 1);
        } while (next_y_shoot_target == enemy.y_shoot_target_);
        enemy.y_shoot_target_ = next_y_shoot_target;
    }

    #undef kDiff
}

// make state for current frame of the game
void game_t::update_state()
{
    monitor_.clearBuffer(); // clear full frame

    // draw stars
    for (size_t i = 0; i < sizeof(stars) / sizeof(stars[0]); i++)
    {
        monitor_.drawPixel(stars[i][0], stars[i][1]);
    }

    update_player_pos();
    draw_entity(player_);

    update_enemy_state(enemy_);
    draw_entity(enemy_);

    update_bullets();

    monitor_.sendBuffer(); // draw full frame
}

void game_t::play()
{
    while (1)
    {
        if (game_state_ == kHlt)
        {
            read_buttons();
            update_pause_state();
        }
        else if (game_state_ == kRunning)
        {
            read_buttons();
            update_state();
        }
    }
}

int main()
{
    init_controller();
    init_pins_and_interrupts();

    // srand((unsigned int)time(NULL)); // works better without this line

    game_t game{};
    game.play();
}

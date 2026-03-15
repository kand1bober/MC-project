#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "interrupts.h"
#include "game.h"
#include "basics.h"

// flags for buttons(like on dualshock)
volatile bool button_up = false;
volatile bool button_down = false;
volatile bool button_left = false;
volatile bool button_right = false;
volatile bool button_cross = false;

void game_t::reset_buttons() {
    uint8_t oldSREG = SREG;
    cli();

    SREG = oldSREG;
}

void game_t::read_buttons()
{
    uint8_t oldSREG = SREG;
    cli();

    // crit section
    if (button_up) {
        buttons_vector_.up = true;
    }
    if (button_down) {
        buttons_vector_.down = true;
    }
    if (button_left) {
        buttons_vector_.left = true;
    }
    if (button_right) {
        buttons_vector_.right = true;
    }
    if (button_cross) {
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

void game_t::end_game(game_result_t result) {
    const int cycles = 70;
    char str1[10];
    char str2[16];
    switch (result) {
        case kDefeat: {
            snprintf(str1, sizeof(str1), "You lost");
            snprintf(str2, sizeof(str2), "            :(");
            break;
        }
        case kVictory: {
            snprintf(str1, sizeof(str1), "You won");
            snprintf(str2, sizeof(str2), "           :)");
            break;
        }
    }

    for (int i = 0; i < cycles; i++) {
        game_t::monitor_.setFont(u8g2_font_tenthinnerguys_tr);
        game_t::monitor_.drawStr(20, 42, str1);
        game_t::monitor_.setFont(u8g2_font_celibatemonk_tr);
        game_t::monitor_.drawStr(20, 42, str2);

        monitor_.sendBuffer(); // draw full frame
    }

    game_t::game_state_ = game_state_t::kHlt;
    game_t::gameTimer.reset();
}

// update current bullet collisions with game units
void game_t::update_collisions(bullet_t &bullet)
{
    entity_t* entities[] = { &enemy_, &player_ };
    
    for (int i = 0; i < sizeof(entities) / sizeof(entities[0]); i++) {
        entity_t* entity = entities[i];
        if (bullet.x_ + bullet.w_ >= entity->x_ &&
            bullet.x_ + bullet.w_ <= entity->x_ + entity->w_ &&
            bullet.y_ >= entity->y_ &&
            bullet.y_ <= entity->y_ + entity->h_)
        {

            // draw explosion
            bitmap_t explosion(explosion_r7, 15, 15);
            draw_bitmap(entity->x_ - 3,
                        entity->y_ - 3,
                        explosion);

            // remove bullet
            bullet.active_ = false;

            if (--(entity->hp_) <= 0)
            {  
                game_result_t result;
                switch (entity->entity_type_) {
                    case kPlayer: {
                        result = kDefeat;
                        break;
                    }
                    case kEnemy: {
                        result = kVictory;
                        break;
                    }
                }
                end_game(result);
            }
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
        player_.y_ = player_.y_ % game_t::display_h;
    }
    if (buttons_vector_.down)
    {
        buttons_vector_.down = false;
        player_.y_ += kSpeed;
        player_.y_ = player_.y_ % game_t::display_h;
    }
    if (buttons_vector_.left)
    {
        buttons_vector_.left = false;
        player_.x_ -= kSpeed;
        player_.x_ = player_.x_ % game_t::display_w;
    }
    if (buttons_vector_.right)
    {
        buttons_vector_.right = false;
        player_.x_ += kSpeed;
        player_.x_ = player_.x_ % game_t::display_w;
    }
    if (buttons_vector_.cross)
    {
        buttons_vector_.cross = false;
        shoot(player_.x_ + player_.bitmap.w_,
              player_.y_ + player_.bitmap.h_ / 2,
              3, 1,
              10, 0);
    }
}

void game_t::update_enemy_state(entity_t &enemy)
{
    #define kVerticalVelocity 2

    // update y position
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
            enemy.vy_ = -kVerticalVelocity  ;
        }
        else {
            enemy.vy_ = kVerticalVelocity;
        }
    }
    else {
        enemy.y_ += enemy.vy_;
    }

    // // show info
    // game_t::monitor_.setFont(u8g2_font_tenthinnerguys_tr);
    // char str[8];
    // sprintf(str, "x= %d", enemy_.x_);
    // game_t::monitor_.drawStr(15, 60, str);

    // update shooting
    if (abs(enemy.y_shoot_target_ - enemy.y_ ) < kVerticalVelocity)
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

    // update x position
    #define kHorizontalVelocity 5

    static uint8_t old_secs = 0;
    uint8_t new_secs = game_t::gameTimer.getSeconds();
    if (new_secs != old_secs && !(new_secs % kHorizontalVelocity)) {
        old_secs = new_secs;
        enemy.x_ -= enemy.bitmap.w_;
    }

    // end game, when enemy reaches left bound
    if (abs(game_t::enemy_.x_) < player_.bitmap.w_) {
        game_t::end_game(kDefeat);
    }

    #undef kHorizontalVelocity
    #undef kVerticalVelocity
}

void game_t::update_visual_info_state() {

    // show hp
    game_t::monitor_.setFont(u8g2_font_tenthinnerguys_tr);
    game_t::monitor_.drawStr(40, 12, "hp");

    char str[10];
    snprintf(str, sizeof(str), "    %u : %u", game_t::player_.hp_, game_t::enemy_.hp_);
    game_t::monitor_.drawStr(40, 12, str);

    // show timer
    gameTimer.display(game_t::monitor_, 49, 63);
}

// make state for current frame of the game
void game_t::update_state()
{
    game_t::gameTimer.update(); // update of timer, should be done every frame

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

    update_visual_info_state();

    monitor_.sendBuffer(); // draw full frame
}

void game_t::play()
{
    while (1)
    {
        if (game_state_ == kHlt)
        {
            read_buttons();
            reset_buttons();
            update_pause_state();
        }
        else if (game_state_ == kRunning)
        {
            read_buttons();
            reset_buttons();
            update_state();
        }
    }
}

int main()
{
    game_t game{};
    game.play();
}

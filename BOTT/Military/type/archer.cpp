#include "archer.h"
#include "constants.h"
#include <QTimer>


Archer::Archer()
{
    timer = new QTimer();
}

Archer::Archer(const Troop & i)
{
    this->setPixmap(i.pixmap());
    this->setTransform(i.transform());
    this->attack = i.getAttack();
    this->hp = i.getHp();
    this->side = i.getSide();
    if(this->side == ConflictSide::Left)
        this->setPos(troops_start_pos::LEFT_START_POSITION_X, troops_start_pos::START_POSITION_Y);
    else
        this->setPos(troops_start_pos::RIGHT_START_POSITION_X, troops_start_pos::START_POSITION_Y);

    this->initialText();
    this->status = Status::Run;
    this->animationCounter = 1;
    this->imgPrefix = i.getImgPrefix();
    this->timer_interval = i.getTimerInterval();
    this->timer_remainingTime = i.getTimerRemainingTime();
    this->type = i.getType();

    timer = new QTimer();

    connect(timer, &QTimer::timeout, this, &Archer::animation);
    connect(timer, &QTimer::timeout, this, &Archer::run);
}

void Archer::animation()
{
    timer->start(timer_interval);

    if(this->status == Status::Stand)
        this->setPixmap(QPixmap(imgPrefix + "stand_1.png"));
    else if(this->status == Status::Run)
    {
        if(this->animationCounter > troops_animation::ARCHER_RUN_ANIMATION_SLIDES_COUNT)
            this->animationCounter = 1;
        this->setPixmap(QPixmap(imgPrefix + "run_" + QString::number(this->animationCounter) + ".png"));
        this->animationCounter++; 
    }
    else if(this->status == Status::Attack)
    {
        if(this->animationCounter > troops_animation::ARCHER_ATTACK_ANIMATION_SLIDES_COUNT)
            this->animationCounter = 1;
        this->setPixmap(QPixmap(imgPrefix + "attack_" + QString::number(this->animationCounter) + ".png"));
        this->animationCounter++;       
    }
}

void Archer::run()
{
    timer->start(timer_interval);

    if(this->status != Status::Run)
        return;
    if(this->side == ConflictSide::Left)
    {
        this->setPos(this->x() + troops_animation::TROOPS_STEP_WIDTH, this->y());
        this->hpText->setPos(this->x() + this->pixmap().width() / 2, this->y() - troops_animation::TROOPS_HP_TEXT_HEIGHT_UNDER_TROOP);
    }
    else
    {
        this->setPos(this->x() - troops_animation::TROOPS_STEP_WIDTH, this->y());
        this->hpText->setPos(this->x() + this->pixmap().width() / 3, this->y() - troops_animation::TROOPS_HP_TEXT_HEIGHT_UNDER_TROOP);
    }
}

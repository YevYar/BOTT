#include "view.h"
#include "gamemenuhandler.h"
#include <QResizeEvent>
#include <QDebug>
#include <QTimer>

View::View(bool bottomView, QWidget * parent) : QGraphicsView(parent)
{
    this->bottomView = bottomView;

    if(bottomView)
    {
        lastSceneX = 1255;
        gameMenu = NULL;
        gameMenu_2 = new GameMenuHandler();

        connect(gameMenu_2, SIGNAL(closeMenu()), this, SLOT(hideMenu()));
    }
    else
    {
        gameMenu = new GameMenuHandler();
        gameMenu_2 = NULL;

        connect(gameMenu, SIGNAL(closeMenu()), this, SLOT(hideMenu()));
    }

    canMenuOpen = true;
    menuOpen = false;

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    inMenuTimer = new QTimer();
    pauseMenuTimer = new QTimer();

    connect(inMenuTimer, SIGNAL(timeout()), this, SLOT(hideMenu()));
    connect(pauseMenuTimer, SIGNAL(timeout()), this, SLOT(setCanMenuOpenInTrue()));
}

View::~View()
{
    if(bottomView)
        delete gameMenu_2;
    else
        delete gameMenu;

    delete inMenuTimer;
    delete pauseMenuTimer;
}

void View::setConfiguration()
{
    if(bottomView)
    {
        controlKeys.insert("menu", Qt::Key_M);
        controlKeys.insert("menu up", Qt::Key_Up);
        controlKeys.insert("menu down", Qt::Key_Down);
        controlKeys.insert("menu select", Qt::Key_Return);
        controlKeys.insert("exit from menu", Qt::Key_Backspace);

        gameMenu_2->setPos(scene()->width() - gameMenu_2->pixmap().width(), 0);
        gameMenu_2->setFlags(QGraphicsItem::ItemIsFocusable);
        gameMenu_2->setVisible(false);
        scene()->addItem(gameMenu_2);
        gameMenu_2->addMenusToScene();
    }
    else
    {
        controlKeys.insert("menu", Qt::Key_Q);
        controlKeys.insert("menu up", Qt::Key_W);
        controlKeys.insert("menu down", Qt::Key_S);
        controlKeys.insert("menu select", Qt::Key_F);
        controlKeys.insert("exit from menu", Qt::Key_E);

        gameMenu->setPos(0, 0);
        gameMenu->setFlags(QGraphicsItem::ItemIsFocusable);
        gameMenu->setVisible(false);
        scene()->addItem(gameMenu);
        gameMenu->addMenusToScene();
    }
}

bool View::isCanMenuOpen()
{
    return canMenuOpen;
}

void View::hideMenu()
{
    // прячем меню и переводим нужные переменные в начальные значения

    if(bottomView)
    {
        gameMenu_2->setVisible(false);
        gameMenu_2->hideCurrentOpenMenu();
    }
    if(!bottomView)
    {
        gameMenu->setVisible(false);
        gameMenu->hideCurrentOpenMenu();
    }

    canMenuOpen = false;
    menuOpen = false;

    emit menuVisibleStatusChanged(this);

    inMenuTimer->stop();
    pauseMenuTimer->start(10000);
}

void View::setCanMenuOpenInTrue()
{
    canMenuOpen = true;
    pauseMenuTimer->stop();
}

void View::resizeEvent(QResizeEvent *event)
{
    // В этом if настраивается sceneRect нижнего view после события View::resizeEvent
    if(bottomView)
    {
        if(event->size().width() == 1256)
        {
            lastSceneX = 1255;
            setSceneRect(1255, 0, 1255, 343);
            return;
        }
        setSceneRect(lastSceneX - (event->size().width() - event->oldSize().width()), 0, scene()->width() - lastSceneX , 343);
        lastSceneX = lastSceneX - (event->size().width() - event->oldSize().width());
    }
}

void View::keyPressEvent(QKeyEvent *event)
{
    if(bottomView)
    {
        if(canMenuOpen && (event->nativeVirtualKey() == getControlKey("menu") || event->key() == getControlKey("menu")))
        {
            if(menuOpen)
                return;

            if(gameMenu_2->scene() != this->scene())
                scene()->addItem(gameMenu_2);

            gameMenu_2->setVisible(true);
            gameMenu_2->setFocus();
            gameMenu_2->showMainMenu();

            inMenuTimer->start(30000);
            menuOpen = true;
            return;
        }
        if(menuOpen)
        {
            if(event->nativeVirtualKey() == getControlKey("menu up") || event->key() == getControlKey("menu up"))
                gameMenu_2->setCurrentItem(true);
            if(event->nativeVirtualKey() == getControlKey("menu down") || event->key() == getControlKey("menu down"))
                gameMenu_2->setCurrentItem(false);
            if(event->nativeVirtualKey() == getControlKey("menu select") || event->key() == getControlKey("menu select"))
                gameMenu_2->processSelectAction();
            if(event->nativeVirtualKey() == getControlKey("exit from menu") || event->key() == getControlKey("exit from menu"))
                gameMenu_2->processExitAction();
        }
    }
    else
    {
        if(canMenuOpen && (event->nativeVirtualKey() == getControlKey("menu") || event->key() == getControlKey("menu")))
        {
            if(menuOpen)
                return;

            if(gameMenu->scene() != this->scene())
                scene()->addItem(gameMenu);

            gameMenu->setVisible(true);
            gameMenu->setFocus();
            gameMenu->showMainMenu();
            inMenuTimer->start(30000);
            menuOpen = true;
            return;
        }
        if(menuOpen)
        {
            if(event->nativeVirtualKey() == getControlKey("menu up") || event->key() == getControlKey("menu up"))
                gameMenu->setCurrentItem(true);
            if(event->nativeVirtualKey() == getControlKey("menu down") || event->key() == getControlKey("menu down"))
                gameMenu->setCurrentItem(false);
            if(event->nativeVirtualKey() == getControlKey("menu select") || event->key() == getControlKey("menu select"))
                gameMenu->processSelectAction();
            if(event->nativeVirtualKey() == getControlKey("exit from menu") || event->key() == getControlKey("exit from menu"))
                gameMenu->processExitAction();
        }
    }
}

bool View::event(QEvent *event)
{
    //qDebug() << "View event " << bottomView;
    //qDebug() << event->type();

    if (event->type() == QEvent::KeyPress)
    {
        // Проверка, что нажатая клавиша - клавиша управления игрока

        if(isControlKey(((QKeyEvent *)event)->nativeVirtualKey()) || isControlKey(((QKeyEvent *)event)->key()))
            keyPressEvent((QKeyEvent *)event);
        return true;
    }
    return QGraphicsView::event(event);
}

bool View::isControlKey(quint32 key)
{
    foreach (Qt::Key value, controlKeys) {
        if(value == key)
            return true;
    }
    return false;
}

bool View::isControlKey(int key)
{
    foreach (Qt::Key value, controlKeys) {
        if(value == key)
            return true;
    }
    return false;
}

Qt::Key View::getControlKey(QString key)
{
    return controlKeys[key];
}
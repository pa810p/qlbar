
#include "gtest/gtest.h"

#include "../src/qlitem.h"

#include <vector>

#define ICON_PATH "/usr/local/share/pixmaps/linux-icon.xpm"
#define EXEC_PATH "aterm -bg black -fg white -g 140x50 -fn -adobe-courier-medium-r-normal--12-120-75-75-m-70-iso8859-2"
#define APP_NAME  "aterm"


TEST(QLItemTest, CreateDestroy) {

    QLItem *item = new QLItem(ICON_PATH, EXEC_PATH, APP_NAME);

    delete item;

}


TEST(QLItemTest, CreateMany) {

    vector<QLItem *> container;

    for (int i=0;i<100;i++) {
        QLItem *item = new QLItem(ICON_PATH, EXEC_PATH, APP_NAME);

        container.push_back(item);
    }

    vector<QLItem *>::iterator it;

    for (it=container.begin();it!=container.end();it++) {
        QLItem * item = (*it);
        delete item;
        item = NULL;
    }

}

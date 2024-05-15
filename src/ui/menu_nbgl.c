
/*****************************************************************************
 *   Ledger App Klaytn.
 *   (c) 2024 Blooo SAS.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

#ifdef HAVE_NBGL

#include "os.h"
#include "glyphs.h"
#include "nbgl_use_case.h"

#include "../globals.h"
#include "menu.h"

//  -----------------------------------------------------------
//  ----------------------- HOME PAGE -------------------------
//  -----------------------------------------------------------

void app_quit(void) {
    // exit app here
    os_sched_exit(-1);
}

// home page definition
void ui_menu_main(void) {
// This parameter shall be set to false if the settings page contains only information
// about the application (version , developer name, ...). It shall be set to
// true if the settings page also contains user configurable parameters related to the
// operation of the application.
#define SETTINGS_BUTTON_ENABLED (true)

    nbgl_useCaseHome(APPNAME,
                     &C_app_klaytn_64px,
                     NULL,
                     SETTINGS_BUTTON_ENABLED,
                     ui_menu_settings,
                     app_quit);
}

//  -----------------------------------------------------------
//  --------------------- SETTINGS MENU -----------------------
//  -----------------------------------------------------------

static const char* const INFO_TYPES[] = {"Version", "Developer"};
static const char* const INFO_CONTENTS[] = {APPVERSION, "Blooo"};

static bool nav_callback(uint8_t page, nbgl_pageContent_t* content) {
    UNUSED(page);

    // the first settings page contains only the version and the developer name
    // of the app (shall be always on the first setting page)
    if (page == 0) {
        content->type = INFOS_LIST;
        content->infosList.nbInfos = 2;
        content->infosList.infoTypes = INFO_TYPES;
        content->infosList.infoContents = INFO_CONTENTS;
    } else {
        return false;
    }
    // valid page so return true
    return true;
}

static void controls_callback(int token, uint8_t index) {
    UNUSED(index);
    UNUSED(token);
}

// settings menu definition
void ui_menu_settings() {
#define TOTAL_SETTINGS_PAGE  (1)
#define INIT_SETTINGS_PAGE   (0)
#define DISABLE_SUB_SETTINGS (false)
    nbgl_useCaseSettings(APPNAME,
                         INIT_SETTINGS_PAGE,
                         TOTAL_SETTINGS_PAGE,
                         DISABLE_SUB_SETTINGS,
                         ui_menu_main,
                         nav_callback,
                         controls_callback);
}

#endif

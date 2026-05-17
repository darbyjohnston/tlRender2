// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/FileActions.h>

#include <tl/PlayerApp/App.h>
#include <tl/PlayerApp/FilesModel.h>

namespace tl
{
    namespace player_app
    {
        struct FileActions::Private
        {
            std::map<std::string, std::shared_ptr<ftk::Action> > actions;

            std::shared_ptr<ftk::Observer<std::shared_ptr<render::Session>>> sessionObserver;
        };

        void FileActions::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            FTK_P();
            auto appWeak = std::weak_ptr<App>(app);
            p.actions["Open"] = ftk::Action::create(
                "Open",
                "FileOpen",
                ftk::KeyShortcut(ftk::Key::O, static_cast<int>(ftk::commandKeyModifier)),
                [appWeak]
                {
                    if (auto app = appWeak.lock())
                    {
                        app->open();
                    }
                });
            p.actions["Open"]->setTooltip("Open a file");

            p.actions["Close"] = ftk::Action::create(
                "Close",
                "FileClose",
                ftk::KeyShortcut(ftk::Key::E, static_cast<int>(ftk::commandKeyModifier)),
                [appWeak]
                {
                    if (auto app = appWeak.lock())
                    {
                        app->close();
                    }
                });
            p.actions["Close"]->setTooltip("Close the current file");

            p.actions["Exit"] = ftk::Action::create(
                "Exit",
                ftk::KeyShortcut(ftk::Key::Q, static_cast<int>(ftk::commandKeyModifier)),
                [appWeak]
                {
                    if (auto app = appWeak.lock())
                    {
                        app->exit();
                    }
                });

            p.sessionObserver = ftk::Observer<std::shared_ptr<render::Session> >::create(
                app->getFilesModel()->observeCurrent(),
                [this](const std::shared_ptr<render::Session>& session)
                {
                    FTK_P();
                    p.actions["Close"]->setEnabled(session.get());
                });
        }

        FileActions::FileActions() :
            _p(new Private)
        {}

        FileActions::~FileActions()
        {}

        std::shared_ptr<FileActions> FileActions::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            auto out = std::shared_ptr<FileActions>(new FileActions);
            out->_init(context, app);
            return out;
        }

        const std::map<std::string, std::shared_ptr<ftk::Action> >& FileActions::getActions() const
        {
            return _p->actions;
        }
    }
}

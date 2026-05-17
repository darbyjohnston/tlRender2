// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Render/Session.h>

namespace tl
{
    namespace player_app
    {
        //! File actions.
        class TL_API_TYPE FilesModel : public std::enable_shared_from_this<FilesModel>
        {
            FTK_NON_COPYABLE(FilesModel);

        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            FilesModel();

        public:
            ~FilesModel();

            //! Create a new model.
            static std::shared_ptr<FilesModel> create(
                const std::shared_ptr<ftk::Context>&);

            //! Get the files.
            const std::vector<std::shared_ptr<render::Session>>& getFiles() const;
            
            //! Observe the files.
            std::shared_ptr<ftk::IObservableList<std::shared_ptr<render::Session>>> observeFiles() const;
            
            //! Add a file.
            void addFile(const std::shared_ptr<render::Session>&);
            
            //! Observe added files.
            std::shared_ptr<ftk::IObservable<int>> observeFileAdd() const;

            //! Remove a file.
            void removeFile(int);

            //! Observe removed files.
            std::shared_ptr<ftk::IObservable<int>> observeFileRemove() const;
            
            //! Get the current file.
            std::shared_ptr<render::Session> getCurrent() const;

            //! Observe the current file.
            std::shared_ptr<ftk::IObservable<std::shared_ptr<render::Session>>> observeCurrent() const;
            
            //! Set the current file.
            void setCurrent(const std::shared_ptr<render::Session>&);

            //! Get the current file.
            int getCurrentIndex() const;

            //! Observe the current file.
            std::shared_ptr<ftk::IObservable<int>> observeCurrentIndex() const;
            
            //! Set the current file.
            void setCurrentIndex(int);

        private:
            FTK_PRIVATE();
        };
    }
}

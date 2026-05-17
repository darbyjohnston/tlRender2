// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/FilesModel.h>

namespace tl
{
    namespace player_app
    {
        struct FilesModel::Private
        {
            std::shared_ptr<ftk::ObservableList<std::shared_ptr<render::Session>>> files;
            std::shared_ptr<ftk::Observable<int>> fileAdd;
            std::shared_ptr<ftk::Observable<int>> fileRemove;
            std::shared_ptr<ftk::Observable<std::shared_ptr<render::Session>>> current;
            std::shared_ptr<ftk::Observable<int>> currentIndex;
        };

        void FilesModel::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();
            p.files = ftk::ObservableList<std::shared_ptr<render::Session>>::create();
            p.fileAdd = ftk::Observable<int>::create();
            p.fileRemove = ftk::Observable<int>::create();
            p.current = ftk::Observable<std::shared_ptr<render::Session>>::create();
            p.currentIndex = ftk::Observable<int>::create(-1);
        }

        FilesModel::FilesModel() :
            _p(new Private)
        {}

        FilesModel::~FilesModel()
        {}

        std::shared_ptr<FilesModel> FilesModel::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            auto out = std::shared_ptr<FilesModel>(new FilesModel);
            out->_init(context);
            return out;
        }

        const std::vector<std::shared_ptr<render::Session> >& FilesModel::getFiles() const
        {
            return _p->files->get();
        }

        std::shared_ptr<ftk::IObservableList<std::shared_ptr<render::Session>>> FilesModel::observeFiles() const
        {
            return _p->files;
        }
        
        void FilesModel::addFile(const std::shared_ptr<render::Session>& file)
        {
            FTK_P();
            const int index = static_cast<int>(p.files->getSize());
            p.files->pushBack(file);
            p.fileAdd->setIfChanged(index);
            p.current->setIfChanged(file);
            p.currentIndex->setIfChanged(index);
        }
        
        std::shared_ptr<ftk::IObservable<int>> FilesModel::observeFileAdd() const
        {
            return _p->fileAdd;
        }

        void FilesModel::removeFile(int index)
        {
            FTK_P();
            if (index >= 0 && index < static_cast<int>(p.files->getSize()))
            {
                p.files->removeItem(index);
                p.fileRemove->setIfChanged(index);
                int current = p.currentIndex->get();
                if (index <= current)
                {
                    --current;
                    const auto& files = p.files->get();
                    p.current->setIfChanged(
                        current >= 0 && current < files.size() ?
                        files[current] :
                        nullptr);
                    p.currentIndex->setIfChanged(current);
                }
            }
        }

        std::shared_ptr<ftk::IObservable<int>> FilesModel::observeFileRemove() const
        {
            return _p->fileRemove;
        }

        std::shared_ptr<render::Session> FilesModel::getCurrent() const
        {
            return _p->current->get();
        }

        std::shared_ptr<ftk::IObservable<std::shared_ptr<render::Session>>> FilesModel::observeCurrent() const
        {
            return _p->current;
        }
        
        void FilesModel::setCurrent(const std::shared_ptr<render::Session>& value)
        {
            FTK_P();
            if (p.current->setIfChanged(value))
            {
                const auto& files = p.files->get();
                const auto i = std::find(files.begin(), files.end(), value);
                p.currentIndex->setIfChanged(
                    i != files.end() ?
                    static_cast<int>(i - files.begin()) :
                    -1);
            }
        }

        int FilesModel::getCurrentIndex() const
        {
            return _p->currentIndex->get();
        }

        std::shared_ptr<ftk::IObservable<int>> FilesModel::observeCurrentIndex() const
        {
            return _p->currentIndex;
        }
        
        void FilesModel::setCurrentIndex(int index)
        {
            FTK_P();
            if (p.currentIndex->setIfChanged(index))
            {
                const auto& files = p.files->get();
                p.current->setIfChanged(
                    index >= 0 && index < static_cast<int>(files.size()) ?
                    files[index] :
                    nullptr);
            }
        }
    }
}

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
        };

        void FilesModel::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();
            p.files = ftk::ObservableList<std::shared_ptr<render::Session>>::create();
            p.fileAdd = ftk::Observable<int>::create();
            p.fileRemove = ftk::Observable<int>::create();
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
        }
        
        std::shared_ptr<ftk::IObservable<int>> FilesModel::observeFileAdd() const
        {
            return _p->fileAdd;
        }

        void FilesModel::removeFile(int index)
        {
            FTK_P();
            if (index >= 0 && index < p.files->getSize())
            {
                p.files->removeItem(index);
                p.fileRemove->setIfChanged(index);
            }
        }

        std::shared_ptr<ftk::IObservable<int>> FilesModel::observeFileRemove() const
        {
            return _p->fileRemove;
        }
    }
}

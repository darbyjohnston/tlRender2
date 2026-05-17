// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <PlayerAppTest/FilesModelTest.h>

#include <tl/PlayerApp/FilesModel.h>

#include <ftk/Core/Assert.h>
#include <ftk/Core/Format.h>

namespace tl
{
    using namespace player_app;

    namespace player_app_test
    {
        FilesModelTest::FilesModelTest(
            const std::shared_ptr<ftk::Context>& context) :
            ITest(context, "tl::player_app_test::FilesModelTest")
        {}

        FilesModelTest::~FilesModelTest()
        {}

        std::shared_ptr<FilesModelTest> FilesModelTest::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<FilesModelTest>(new FilesModelTest(context));
        }

        void FilesModelTest::run()
        {
            auto context = _context.lock();

            const ftk::Path path0(TL_TEST_DATA_DIR, "SingleClip.otio");
            const ftk::Path path1(TL_TEST_DATA_DIR, "MultipleClips.otio");
            const ftk::Path path2(TL_TEST_DATA_DIR, "Overlay.otio");

            auto model = FilesModel::create(context);
            auto session0 = render::Session::create(context, path0);
            auto session1 = render::Session::create(context, path1);
            auto session2 = render::Session::create(context, path2);
            model->addFile(session0);
            model->addFile(session1);
            model->addFile(session2);
            FTK_ASSERT(3 == model->getFiles().size());
            FTK_ASSERT(session2 == model->getCurrent());
            FTK_ASSERT(2 == model->getCurrentIndex());
            
            model->removeFile(0);
            FTK_ASSERT(2 == model->getFiles().size());
            FTK_ASSERT(session2 == model->getCurrent());
            FTK_ASSERT(1 == model->getCurrentIndex());
            
            model->removeFile(0);
            FTK_ASSERT(1 == model->getFiles().size());
            FTK_ASSERT(session2 == model->getCurrent());
            FTK_ASSERT(0 == model->getCurrentIndex());
            
            model->removeFile(0);
            FTK_ASSERT(0 == model->getFiles().size());
            FTK_ASSERT(nullptr == model->getCurrent());
            FTK_ASSERT(-1 == model->getCurrentIndex());

            model->addFile(session0);
            model->addFile(session1);
            model->addFile(session2);
            model->setCurrent(session1);
            model->setCurrent(session1);
            FTK_ASSERT(session1 == model->getCurrent());
            FTK_ASSERT(1 == model->getCurrentIndex());
        }
    }
}

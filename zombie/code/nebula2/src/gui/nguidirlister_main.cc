#include "precompiled/pchngui.h"
//------------------------------------------------------------------------------
//  nguidirlister_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguidirlister.h"
#include "kernel/nfileserver2.h"
#include "kernel/ndirectory.h"

#include "gfx2/nfont2.h"
#include "gui/nguiserver.h"
#include "gui/nguiclientwindow.h"

nNebulaScriptClass(nGuiDirLister, "nguitextview");

//------------------------------------------------------------------------------
/**
*/
nGuiDirLister::nGuiDirLister() :
    dirPath("xxx:"),
    pattern("*"),
    ignoreSubDirs(false),
    ignoreFiles(false),
    dirty(true),
    stripExtension(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiDirLister::~nGuiDirLister()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiDirLister::OnShow()
{
    this->UpdateContent();
    nGuiTextView::OnShow();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiDirLister::OnFrame()
{
    if (this->dirty)
    {
        this->UpdateContent();
    }
    nGuiTextView::OnFrame();
}

//------------------------------------------------------------------------------
/**
    Update the text view with content of directory.
*/
void
nGuiDirLister::UpdateContent()
{
    nDirectory* dir = kernelServer->GetFileServer()->NewDirectoryObject();
    n_assert(dir);
    this->BeginAppend();
    if (dir->Open(this->GetDirectory()))
    {
        if (!this->ignoreSubDirs)
        {
            this->AppendLine("..");
        }
        if (dir->SetToFirstEntry()) do
        {
            nString entryStr = dir->GetEntryName();
            nString entryName = entryStr.ExtractFileName();
            nDirectory::EntryType entryType = dir->GetEntryType();
            if ((nDirectory::DIRECTORY == entryType) && (this->ignoreSubDirs))
            {
                continue;
            }
            if ((nDirectory::FILE == entryType) && (this->ignoreFiles))
            {
                continue;
            }
            if (!n_strmatch(entryName.Get(), this->pattern.Get()))
            {
                continue;
            }
            if ((0 == strcmp(entryName.Get(), ".")) || 
                (0 == strcmp(entryName.Get(), "..")) ||
                (0 == strcmp(entryName.Get(), "CVS")) ||
                (0 == strcmp(entryName.Get(), ".svn")))
            {
                continue;
            }
            // a positive match
            // if in strip extension mode, strip the extension from the file
            if (this->stripExtension)
            {
                entryName.StripExtension();
            }
            this->AppendLine(entryName);
        }
        while (dir->SetToNextEntry());

        dir->Close();
    }
    delete dir;
    this->EndAppend();
    this->SetLineOffset(0);
    this->dirty = false;
}

// overriding render method
// (we'll try to light the selection as mouse is over them)
bool 
nGuiDirLister::Render()
{
    if (this->IsShown())
    {
        // render background
        nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), this->defaultBrush);

        int beginIndex = this->lineOffset;
        int endIndex = beginIndex + this->GetNumVisibleLines();
        if (endIndex > this->textArray.Size())
        {
            endIndex = this->textArray.Size();
        }
        if ((endIndex - beginIndex) > 0)
        {
            nGfxServer2* gfxServer = nGfxServer2::Instance();
            gfxServer->SetFont(this->refFont.get());

            // add some border tolerance
            rectangle screenSpaceRect = this->GetScreenSpaceRect();
            rectangle curRect = screenSpaceRect;            
            curRect.v1.y = curRect.v0.y + this->lineHeight;

            // add border for text
            rectangle curTextRect = curRect;
            curTextRect.v0.x += 0.005f;
            curTextRect.v1.x -= 0.005f;

            const int renderFlags = nFont2::Left | nFont2::ExpandTabs;
            int i;
            for (i = beginIndex; i < endIndex; i++)
            {
                
                // highlight option under mouse?
                if (curTextRect.inside(nGuiServer::Instance()->GetMousePos())) {
                    nGuiServer::Instance()->DrawBrush(curRect, this->highlightBrush);
                }

                // is option selected?
                if (this->selectionEnabled && (i == this->selectionIndex))
                {
                    //nGuiServer::Instance()->DrawBrush(curRect, this->pressedBrush);
                    nGuiServer::Instance()->DrawText(this->textArray[i].Get(), vector4(0.7f,0.0f,0.0f,1.0f), curTextRect, renderFlags);
                }
                else
                {
                    nGuiServer::Instance()->DrawText(this->textArray[i].Get(), vector4(0.0f,0.0f,0.0f,1.0f), curTextRect, renderFlags);
                }
//              nGuiServer::Instance()->DrawText(this->textArray[i].Get(), this->textColor, curTextRect, renderFlags);
                curRect.v0.y = curRect.v1.y;
                curTextRect.v0.y = curTextRect.v1.y;
                curRect.v1.y += this->lineHeight;
                curTextRect.v1.y += this->lineHeight;
            }
        }
        
        // do *not* render parent class to avoid standard-render from nguitextview
        //return nGuiTextView::Render();

        // instead, render FormLayout since we can have a slider
        return nGuiFormLayout::Render();
    }
    return false;
}

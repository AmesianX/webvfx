// Copyright (c) 2010 Hewlett-Packard Development Company, L.P. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMIX_MIXRENDER_H_
#define CHROMIX_MIXRENDER_H_

#include "chromix/Loader.h"
#include "chromix/ParameterValue.h"

#include <base/basictypes.h>
#include <skia/ext/platform_canvas.h>
#include <third_party/WebKit/WebKit/chromium/public/WebSize.h>

namespace WTF {
    class String;
}

namespace Chromix {

class ScriptingSupport;

class MixRender {
public:
    MixRender();
    virtual ~MixRender();

    void setLogger(LogCallback logger, const void* data = NULL) { loader.setLogger(logger, data); }

    bool loadURL(const WTF::String& url);
    void resize(int width, int height);
    const SkBitmap* render(double time);

    void setParameterValue(const WTF::String& name, bool value);
    void setParameterValue(const WTF::String& name, double value);
    void setParameterValue(const WTF::String& name, const WTF::String& value);
    unsigned char* writeableDataForImageParameter(const WTF::String& name, unsigned int width, unsigned int height);

private:
    WebKit::WebView *webView;
    WebKit::WebSize size;
    skia::PlatformCanvas* skiaCanvas;
    Loader loader;
    ScriptingSupport *scriptingSupport;

    DISALLOW_COPY_AND_ASSIGN(MixRender);
};
    
}

#endif

//
// Frame.hh for pekwm
// Copyright (C) 2003-2020 Claes Nästén <pekdon@gmail.com>
//
// This program is licensed under the GNU GPL.
// See the LICENSE file for more information.
//

#ifndef _FRAME_HH_
#define _FRAME_HH_

#include "config.h"

#include "pekwm.hh"
#include "Action.hh"
#include "PDecor.hh"
#include "Client.hh"

class PWinObj;
class Strut;
class ClassHint;
class AutoProperty;

#include <string>

class Frame : public PDecor
{
public:
    Frame(Client *client, AutoProperty *ap);
    virtual ~Frame(void);

    // START - PWinObj interface.
    virtual void iconify(void);
    virtual void stick(void);

    virtual void setWorkspace(unsigned int workspace);
    virtual void setLayer(Layer layer);

    virtual ActionEvent *handleMotionEvent(XMotionEvent *ev);
    virtual ActionEvent *handleEnterEvent(XCrossingEvent *ev);
    virtual ActionEvent *handleLeaveEvent(XCrossingEvent *ev);

    virtual ActionEvent *handleMapRequest(XMapRequestEvent *ev);
    virtual ActionEvent *handleUnmapEvent(XUnmapEvent *ev);
    // END - PWinObj interface.

#ifdef HAVE_SHAPE
    virtual void handleShapeEvent(XShapeEvent *ev);
#endif

    // START - PDecor interface.
    virtual bool allowMove(void) const;

    virtual void addChild(PWinObj *child, vector<PWinObj*>::iterator *it = 0);
    virtual void removeChild(PWinObj *child, bool do_delete = true);
    virtual void activateChild(PWinObj *child);

    virtual void updatedChildOrder(void);
    virtual void updatedActiveChild(void);

    virtual void getDecorInfo(wchar_t *buf, uint size);

    virtual void giveInputFocus(void);
    virtual void setShaded(StateAction sa);
    virtual void setSkip(uint skip);
    // END - PDecor interface.

    Client *getActiveClient(void);

    void addChildOrdered(Client *child);

    static Frame *findFrameFromWindow(Window win);
    static Frame *findFrameFromID(uint id);

    // START - Iterators
    static uint frame_size(void) { return _frames.size(); }
    static vector<Frame*>::const_iterator frame_begin(void) {
        return _frames.begin();
    }
    static vector<Frame*>::const_iterator frame_end(void) {
        return _frames.end();
    }
    static vector<Frame*>::const_reverse_iterator frame_rbegin(void) {
        return _frames.rbegin();
    }
    static vector<Frame*>::const_reverse_iterator frame_rend(void) {
        return _frames.rend();
    }

    Client *getTransFor(void) const {
        return _client?_client->getTransientForClient():0;
    }
    bool hasTrans(void) const {
        return _client && _client->hasTransients();
    }
    // Call getTransBegin() only (!) if hasTrans() == true.
    vector<Client*>::const_iterator getTransBegin(void) const {
        return _client->getTransientsBegin();
    }
    // Call getTransEnd() only (!) if hasTrans() == true.
    vector<Client*>::const_iterator getTransEnd(void) const {
        return _client->getTransientsEnd();
    }
    // END - Iterator

    inline uint getId(void) const { return _id; }
    void setId(uint id);

    void detachClient(Client *client);

    inline const ClassHint* getClassHint(void) const { return _class_hint; }

    void setGeometry(const std::string geometry, int head=-1);
    void setGeometry(const Geometry &geometry, int gm_mask, int head=-1);

    void growDirection(uint direction);
    void moveToHead(int head_nr);
    void moveToEdge(OrientationType ori);

    void updateInactiveChildInfo(void);

    // state actions
    void setStateMaximized(StateAction sa, bool horz, bool vert, bool fill);
    void setStateFullscreen(StateAction sa);
    void setStateSticky(StateAction sa);
    void setStateAlwaysOnTop(StateAction sa);
    void setStateAlwaysBelow(StateAction sa);
    void setStateDecorBorder(StateAction sa);
    void setStateDecorTitlebar(StateAction sa);
    void setStateIconified(StateAction sa);
    void setStateTagged(StateAction sa, bool behind);
    void setStateSkip(StateAction sa, uint skip);
    void setStateTitle(StateAction sa, Client *client, const std::wstring &title);
    void setStateMarked(StateAction sa, Client *client);
    void setStateOpaque(StateAction sa);

    void close(void);

    void readAutoprops(ApplyOn type = APPLY_ON_RELOAD);

    void doResize(XMotionEvent *ev); // redirects to doResize(bool...
    void doResize(BorderPosition pos); // redirect to doResize(bool...
    void doResize(bool left, bool x, bool top, bool y);
    void doGroupingDrag(XMotionEvent *ev, Client *client, bool behind);

    bool fixGeometry(void);

    // client message handling
    void handleConfigureRequest(XConfigureRequestEvent *ev, Client *client);
    void handleClientMessage(XClientMessageEvent *ev, Client *client);
    void handlePropertyChange(XPropertyEvent *ev, Client *client);

    static Frame *getTagFrame(void) { return _tag_frame; }
    static bool getTagBehind(void) { return _tag_behind; }

    static void resetFrameIDs(void);

protected:
    // BEGIN - PDecor interface
    virtual int resizeHorzStep(int diff) const;
    virtual int resizeVertStep(int diff) const;

    std::string getDecorName(void);
    // END - PDecor interface

private:
    void handleClientStateMessage(XClientMessageEvent *ev, Client *client);
    static StateAction getStateActionFromMessage(XClientMessageEvent *ev);
    void handleStateAtom(StateAction sa, Atom atom, Client *client);
    void handleCurrentClientStateAtom(StateAction sa, Atom atom, Client *client);
    bool isRequestGeometryFullscreen(XConfigureRequestEvent *ev);

    void recalcResizeDrag(int nx, int ny, bool left, bool top);
    void getMaxBounds(int &max_x,int &max_r, int &max_y, int &max_b);
    void calcSizeInCells(uint &width, uint &height);
    void setGravityPosition(int gravity, int &x, int &y, int diff_w, int diff_h);
    void downSize(Geometry &gm, bool keep_x, bool keep_y);

    void handleTitleChange(Client *client);

    void getState(Client *cl);
    void applyState(Client *cl);

    void setupAPGeometry(Client *client, AutoProperty *ap);
    void applyGeometry(Geometry &gm, const Geometry &ap_gm, int mask);
    void applyGeometry(Geometry &gm, const Geometry &ap_gm, int mask, const Geometry &screen_gm);

    void setActiveTitle(void);

    static uint findFrameID(void);
    static void returnFrameID(uint id);

private:
    uint _id; // unique id of the frame

    Client *_client; // to skip all the casts from PWinObj
    ClassHint *_class_hint;

    // frame information used when maximizing / going fullscreen
    Geometry _old_gm; // FIXME: move to PDecor?
    uint _non_fullscreen_decor_state; // FIXME: move to PDecor?
    Layer _non_fullscreen_layer;

    static vector<Frame*> _frames; //!< Vector of all Frames.
    static vector<uint> _frameid_list; //!< Vector of free Frame IDs.

    // Tagging, static as only one Frame can be tagged
    static Frame *_tag_frame; //!< Pointer to tagged frame.
    static bool _tag_behind; //!< Tagging actions will set behind.

    // EWMH
    static const int NET_WM_STATE_REMOVE = 0; // remove/unset property
    static const int NET_WM_STATE_ADD = 1; // add/set property
    static const int NET_WM_STATE_TOGGLE = 2; // toggle property
};

#endif // _FRAME_HH_

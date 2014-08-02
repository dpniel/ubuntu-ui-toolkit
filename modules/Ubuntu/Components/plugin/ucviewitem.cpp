/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ucunits.h"
#include "uctheme.h"
#include "ucviewitem.h"
#include "ucviewitem_p.h"
#include <QtQml/QQmlInfo>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickpositioners_p.h>

/******************************************************************************
 * Divider
 */
UCViewItemDivider::UCViewItemDivider(UCViewItemBase *viewItem)
    : QObject(viewItem)
    , m_visible(true)
    , m_thickness(UCUnits::instance().dp(2))
    , m_leftMargin(UCUnits::instance().gu(2))
    , m_rightMargin(UCUnits::instance().gu(2))
    , m_viewItem(viewItem)
{
}
UCViewItemDivider::~UCViewItemDivider()
{
}

QSGNode *UCViewItemDivider::paint(QSGNode *paintNode, const QRectF &rect)
{
    if (m_visible) {
        QSGRectangleNode *rectNode = static_cast<QSGRectangleNode *>(paintNode);
        if (!rectNode) {
            rectNode = QQuickItemPrivate::get(m_viewItem)->sceneGraphContext()->createRectangleNode();
        }
        rectNode->setRect(QRectF(m_leftMargin, rect.height() - m_thickness,
                                 rect.width() - m_leftMargin - m_rightMargin, m_thickness));
        QGradientStops stops;
        // FIXME: use palette colors!
        stops.append(QGradientStop(0.0, QColor("#26000000")));
        stops.append(QGradientStop(1.0, QColor("#14F3F3E7")));
        rectNode->setGradientStops(stops);
        rectNode->update();
        return rectNode;
    } else {
        delete paintNode;
        return 0;
    }
}
/******************************************************************************
 * ViewItemBackground
 */
UCViewItemBackground::UCViewItemBackground(QQuickItem *parent)
    : QQuickItem(parent)
    , m_color(Qt::transparent)
    , m_pressedColor(Qt::yellow)
    , m_item(0)
{
    setFlag(QQuickItem::ItemHasContents, true);
    // set the z-order to be above the main item
    setZ(1);
}

UCViewItemBackground::~UCViewItemBackground()
{
}

void UCViewItemBackground::itemChange(ItemChange change, const ItemChangeData &data)
{
    if (change == ItemParentHasChanged) {
        m_item = qobject_cast<UCViewItemBase*>(data.item);
    }
    QQuickItem::itemChange(change, data);
}

QSGNode *UCViewItemBackground::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    UCViewItemBasePrivate *dd = UCViewItemBasePrivate::get(m_item);
    bool pressed = (dd && dd->pressed);
    QColor color = pressed ? m_pressedColor : m_color;

    if (width() <= 0 || height() <= 0 || (color.alpha() == 0)) {
        delete oldNode;
        return 0;
    }
    QSGRectangleNode *rectNode = static_cast<QSGRectangleNode *>(oldNode);
    if (!rectNode) {
        rectNode = QQuickItemPrivate::get(this)->sceneGraphContext()->createRectangleNode();
    }
    rectNode->setColor(color);
    rectNode->setRect(boundingRect());
    rectNode->update();
    return rectNode;
}


/******************************************************************************
 * ViewItemBasePrivate
 */
UCViewItemBasePrivate::UCViewItemBasePrivate(UCViewItemBase *qq)
    : q_ptr(qq)
    , background(new UCViewItemBackground)
    , divider(new UCViewItemDivider(q_ptr))
    , leadingOptions(0)
    , trailingOptions(0)
    , pressed(false)
{
    background->setObjectName("ViewItemHolder");
    background->setParent(q_ptr);
    background->setParentItem(q_ptr);
    // content will be redirected to the background, therefore we must report
    // children changes as it would come from the main component
    QObject::connect(background, &UCViewItemBackground::childrenChanged,
                     q_ptr, &UCViewItemBase::childrenChanged);
}

void UCViewItemBasePrivate::_q_rebound()
{
    setPressed(false);
    // disconnect the flickable
    listenToRebind(false);
}

// set pressed flag and update background
void UCViewItemBasePrivate::setPressed(bool pressed)
{
    if (this->pressed != pressed) {
        this->pressed = pressed;
        background->update();
        Q_Q(UCViewItemBase);
        Q_EMIT q->pressedChanged();
    }
}

// connects/disconnects from the Flickable anchestor to get notified when to do rebound
void UCViewItemBasePrivate::listenToRebind(bool listen)
{
    if (flickable.isNull()) {
        return;
    }
    if (listen) {
        QObject::connect(flickable.data(), SIGNAL(movementStarted()), q_ptr, SLOT(_q_rebound()));
    } else {
        QObject::disconnect(flickable.data(), SIGNAL(movementStarted()), q_ptr, SLOT(_q_rebound()));
    }
}

void UCViewItemBasePrivate::resize()
{
    QRectF rect(q_ptr->boundingRect());
    if (divider && divider->m_visible) {
        rect.setHeight(rect.height() - divider->m_thickness);
    }
    background->setSize(rect.size());
}

/*!
 * \qmltype ViewItemBase
 * \instantiates UCViewItemBase
 * \inherits Item
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief The ViewItem element provides Ubuntu design standards for list or grid
 * views.
 *
 * The component is dedicated to be used in designs with static or dynamic lists
 * (i.e. list views where each item's layout differs or in lists where the content
 * is determined by a given model, thus each element has the same layout). The
 * element does not define any specific layout, components can be placed in any
 * ways on it. However, when used in scrolling lists, the content must be chosen
 * carefully to in order to keep the kinetic behavior and the 60 FPS if possible.
 *
 * The component does not set any size, therefore if used these properties should
 * be set. Colors used are also hardcoded ones. Use \l ViewItem instead of this
 * component, which provides bindings to theme palette and aligns to the component
 * is embedded in.
 *
 * \sa ViewItem
 */

/*!
 * \qmlsignal ViewItemBase::clicked()
 *
 * The signal is emitted when the component gets released while the \l pressed property
 * is set. When used in Flickable, the signal is not emitted if when the Flickable gets
 * moved.
 */
UCViewItemBase::UCViewItemBase(QQuickItem *parent)
    : QQuickItem(parent)
    , d_ptr(new UCViewItemBasePrivate(this))
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::LeftButton);
}

UCViewItemBase::~UCViewItemBase()
{
}

void UCViewItemBase::itemChange(ItemChange change, const ItemChangeData &data)
{
    QQuickItem::itemChange(change, data);
    if (change == ItemParentHasChanged) {
        Q_D(UCViewItemBase);
        // make sure we are not connected to the previous Flickable
        if (!d->flickable.isNull()) {
            QObject::disconnect(d->flickable.data(), SIGNAL(movementStarted()), this, SLOT(_q_rebound()));
        }
        // check if we are in a positioner, and if that positioner is in a Flickable
        QQuickBasePositioner *positioner = qobject_cast<QQuickBasePositioner*>(data.item);
        if (positioner && positioner->parentItem()) {
            d->flickable = qobject_cast<QQuickFlickable*>(positioner->parentItem()->parentItem());
            Q_EMIT flickableChanged();
        } else if (data.item && data.item->parentItem()){
            // check if we are in a Flickable then
            d->flickable = qobject_cast<QQuickFlickable*>(data.item->parentItem());
            Q_EMIT flickableChanged();
        }
    }
}

void UCViewItemBase::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    // resize background item
    Q_D(UCViewItemBase);
    d->resize();
}

QSGNode *UCViewItemBase::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);
    Q_D(UCViewItemBase);
    if (width() <= 0 || height() <= 0 || !d->divider) {
        delete oldNode;
        return 0;
    }
    // paint divider
    return d->divider->paint(oldNode, boundingRect());
}

void UCViewItemBase::mousePressEvent(QMouseEvent *event)
{
    QQuickItem::mousePressEvent(event);
    Q_D(UCViewItemBase);
    if (!d->flickable.isNull() && d->flickable->isMoving()) {
        // while moving, we cannot select any items
        return;
    }
    d->setPressed(true);
    // connect the Flickable to know when to rebound
    if (!d->flickable.isNull()) {
        QObject::connect(d->flickable.data(), SIGNAL(movementStarted()), this, SLOT(_q_rebound()));
    }
    // accept the event so we get the rest of the events as well
    event->setAccepted(true);
}

void UCViewItemBase::mouseReleaseEvent(QMouseEvent *event)
{
    QQuickItem::mouseReleaseEvent(event);
    Q_D(UCViewItemBase);
    // set released
    if (d->pressed) {
        Q_EMIT clicked();
    }
    d->setPressed(false);
}

/*!
 * \qmlproperty ViewItemOptions ViewItemBase::leadingOptions
 *
 * The property holds the options and its configuration to be revealed when swiped
 * from left to right.
 */
PROPERTY_GETTER(UCViewItemBase, UCViewItemOptions*, leadingOptions)
PROPERTY_SETTER(UCViewItemBase, UCViewItemOptions*, leadingOptions)

/*!
 * \qmlproperty ViewItemOptions ViewItemBase::trailingOptions
 *
 * The property holds the options and its configuration to be revealed when swiped
 * from right to left.
 */
PROPERTY_GETTER(UCViewItemBase, UCViewItemOptions*, trailingOptions)
PROPERTY_SETTER(UCViewItemBase, UCViewItemOptions*, trailingOptions)

/*!
 * \qmlpropertygroup ::ViewItemBase::background
 * \qmlproperty color ViewItemBase::background.color
 * \qmlproperty color ViewItemBase::background.pressedColor
 *
 * background grouped property is an Item which holds the layout of the item, with
 * abilities to show different colors when in normal state or when pressed. All
 * properties from Item are accessible and can be used to control user defined
 * actions or animations, with the exception of the followings:
 * \list A
 * \li do not alter x, y, width or height properties as those are controlled by the
 *      item itself when leading or trailing options are revealed and will destroy
 *      your logic
 * \li never anchor left or right as it will block revealing the options.
 * \endlist
 */
PROPERTY_GETTER(UCViewItemBase, UCViewItemBackground*, background)

/*!
 * \qmlpropertygroup ::ViewItemBase::divider
 * \qmlproperty bool ViewItemBase::divider.visible
 * \qmlproperty real ViewItemBase::divider.thickness
 * \qmlproperty real ViewItemBase::divider.leftMargin
 * \qmlproperty real ViewItemBase::divider.rightMargin
 *
 * This grouped property configures the thin divider shown in the bottom of the
 * component. Controls the visibility the thickness and the margins from the left
 * and right of the ViewItem. It is not moved togehther with the content when
 * options are revealed.
 *
 * When \c visible is true, the ViewItem's content size gets thinner with the
 * divider's \c thickness.
 */
PROPERTY_GETTER(UCViewItemBase, UCViewItemDivider*, divider)

/*!
 * \qmlproperty bool ViewItemBase::pressed
 * True when the item is pressed. The items stays pressed when the mouse or touch
 * is moved horizontally. When in Flickable (or ListView), the item gets un-pressed
 * (false) when the mouse or touch is moved towards the vertical direction causing
 * the flickable to move.
 */
PROPERTY_GETTER(UCViewItemBase, bool, pressed)

/*!
 * \qmlproperty Flickable ViewItemBase::flickable
 * The property contains the Flickable the component is embedded in. The property is set
 * in the following cases:
 * \qml
 * Flickable {
 *     id: flickableItem
 *     width: units.gu(30)
 *     height: units.gu(30)
 *     contentHeight: column.childrenRect.height
 *     Column {
 *         id: column
 *         Repeater {
 *             model: 100
 *             ViewItem {
 *             }
 *         }
 *     }
 * }
 * \endqml
 * In this case the ViewItem's flickable property points to the \c flickableItem, and
 * parent to the \c column.
 * \qml
 * ListView {
 *     id: listView
 *     width: units.gu(30)
 *     height: units.gu(30)
 *     model: 100
 *     delegate: ViewItem {
 *     }
 * }
 * \endqml
 * In this case the flickable property will contain the \c listView.
 *
 * In any other cases the flickable property will be set to null.
 */
PROPERTY_GETTER(UCViewItemBase, QQuickFlickable *, flickable)

/*!
 * \qmlproperty list<Object> ViewItemBase::data
 * \default
 * Overloaded default property containing all the children and resources.
 */
QQmlListProperty<QObject> UCViewItemBase::data()
{
    Q_D(UCViewItemBase);
    return QQuickItemPrivate::get(d->background)->data();
}

/*!
 * \qmlproperty list<Item> ViewItemBase::children
 * Overloaded default property containing all the visible children of the item.
 */
QQmlListProperty<QQuickItem> UCViewItemBase::children()
{
    Q_D(UCViewItemBase);
    return QQuickItemPrivate::get(d->background)->children();
}

#include "moc_ucviewitem.cpp"

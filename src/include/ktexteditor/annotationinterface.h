/*
    SPDX-FileCopyrightText: 2008 Andreas Pakulat <apaku@gmx.de>
    SPDX-FileCopyrightText: 2008-2018 Dominik Haumann <dhaumann@kde.org>
    SPDX-FileCopyrightText: 2017-2018 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KTEXTEDITOR_ANNOTATIONINTERFACE_H
#define KTEXTEDITOR_ANNOTATIONINTERFACE_H

#include <ktexteditor_export.h>

#include <QObject>

class QMenu;

namespace KTextEditor
{
class View;
class AbstractAnnotationItemDelegate;

/**
 * \brief An model for providing line annotation information
 *
 * \section annomodel_intro Introduction
 *
 * AnnotationModel is a model-like interface that can be implemented
 * to provide annotation information for each line in a document. It provides
 * means to retrieve several kinds of data for a given line in the document.
 *
 * \section annomodel_impl Implementing a AnnotationModel
 *
 * The public interface of this class is loosely based on the QAbstractItemModel
 * interfaces. It only has a single method to override which is the \ref data()
 * method to provide the actual data for a line and role combination.
 *
 * \since 4.1
 * \see KTextEditor::AnnotationInterface, KTextEditor::AnnotationViewInterface
 */
class KTEXTEDITOR_EXPORT AnnotationModel : public QObject
{
    Q_OBJECT
public:
    ~AnnotationModel() override
    {
    }

    enum { GroupIdentifierRole = Qt::UserRole };
    // KF6: add AnnotationModelUserRole = Qt::UserRole + 0x100

    /**
     * data() is used to retrieve the information needed to present the
     * annotation information from the annotation model. The provider
     * should return useful information for the line and the data role.
     *
     * The following roles are supported:
     * - Qt::DisplayRole - a short display text to be placed in the border
     * - Qt::TooltipRole - a tooltip information, longer text possible
     * - Qt::BackgroundRole - a brush to be used to paint the background on the border
     * - Qt::ForegroundRole - a brush to be used to paint the text on the border
     * - AnnotationModel::GroupIdentifierRole - a string which identifies a
     *   group of items which will be highlighted on mouseover; return the same
     *   string for all items in a group (KDevelop uses a VCS revision number, for example)
     *
     *
     * \param line the line for which the data is to be retrieved
     * \param role the role to identify which kind of annotation is to be retrieved
     *
     * \returns a QVariant that contains the data for the given role.
     */
    virtual QVariant data(int line, Qt::ItemDataRole role) const = 0; // KF6: use int for role

Q_SIGNALS:
    /**
     * The model should emit the signal reset() when the text of almost all
     * lines changes. In most cases it is enough to call lineChanged().
     *
     * \note Kate Part implementation details: Whenever reset() is emitted Kate
     *       Part iterates over all lines of the document. Kate Part searches
     *       for the longest text to determine the annotation border's width.
     *
     * \see lineChanged()
     */
    void reset();

    /**
     * The model should emit the signal lineChanged() when a line has to be
     * updated.
     *
     * \note Kate Part implementation details: lineChanged() repaints the whole
     *       annotation border automatically.
     */
    void lineChanged(int line);
};

/**
 * \class AnnotationInterface annotationinterface.h <KTextEditor/AnnotationInterface>
 *
 * \brief A Document extension interface for handling Annotation%s
 *
 * \ingroup kte_group_doc_extensions
 *
 * \section annoiface_intro Introduction
 *
 * The AnnotationInterface is designed to provide line annotation information
 * for a document. This interface provides means to associate a document with a
 * annotation model, which provides some annotation information for each line
 * in the document.
 *
 * Setting a model for a Document makes the model data available for all views.
 * If you only want to provide annotations in exactly one view, you can use
 * the AnnotationViewInterface directly. See the AnnotationViewInterface for
 * further details. To summarize, the two use cases are
 * - (1) show annotations in all views. This means you set an AnnotationModel
 *       with this interface, and then call setAnnotationBorderVisible() for
 *       each view.
 * - (2) show annotations only in one view. This means to \e not use this
 *       interface. Instead, use the AnnotationViewInterface, which inherits
 *       this interface. This means you set a model for the specific View.
 *
 * If you set a model to the Document \e and the View, the View's model has
 * higher priority.
 *
 * \section annoiface_access Accessing the AnnotationInterface
 *
 * The AnnotationInterface is an extension interface for a Document, i.e. the
 * Document inherits the interface \e provided that the
 * used KTextEditor library implements the interface. Use qobject_cast to
 * access the interface:
 * \code
 * // document is of type KTextEditor::Document*
 * auto iface = qobject_cast<KTextEditor::AnnotationInterface*>(document);
 *
 * if (iface) {
 *     // the implementation supports the interface
 *     // do stuff
 * } else {
 *     // the implementation does not support the interface
 * }
 * \endcode
 *
 * \section annoiface_usage Using the AnnotationInterface
 *
 * \since 4.1
 * \see KTextEditor::AnnotationModel, KTextEditor::AnnotationViewInterface
 */
class KTEXTEDITOR_EXPORT AnnotationInterface
{
public:
    virtual ~AnnotationInterface()
    {
    }

    /**
     * Sets a new \ref AnnotationModel for this document to provide
     * annotation information for each line.
     *
     * \param model the new AnnotationModel
     */
    virtual void setAnnotationModel(AnnotationModel *model) = 0;

    /**
     * returns the currently set \ref AnnotationModel or 0 if there's none
     * set
     * @returns the current \ref AnnotationModel
     */
    virtual AnnotationModel *annotationModel() const = 0;
};

/**
 * \brief Annotation interface for the View
 *
 * \ingroup kte_group_view_extensions
 *
 * \section annoview_intro Introduction
 *
 * The AnnotationViewInterface allows to do these things:
 * - (1) show/hide the annotation border along with the possibility to add actions
 *       into its context menu.
 * - (2) set a separate AnnotationModel for the View: Note that this interface
 *       inherits the AnnotationInterface.
 * - (3) set a custom AbstractAnnotationItemDelegate for the View.
 *
 * For a more detailed explanation about whether you want an AnnotationModel
 * in the Document or the View, read the detailed documentation about the
 * AnnotationInterface.
 *
 * For a more detailed explanation about whether you want to set a custom
 * delegate for rendering the annotations, read the detailed documentation about the
 * AbstractAnnotationItemDelegate.
 *
 * \section annoview_access Accessing the AnnotationViewInterface
 *
 * The AnnotationViewInterface is an extension interface for a
 * View, i.e. the View inherits the interface \e provided that the
 * used KTextEditor library implements the interface. Use qobject_cast to
 * access the interface:
 * \code
 * // view is of type KTextEditor::View*
 * auto iface = qobject_cast<KTextEditor::AnnotationViewInterface*>(view);
 *
 * if (iface) {
 *     // the implementation supports the interface
 *     // do stuff
 *     iface->setAnnotationBorderVisible(true);
 *     iface->setAnnotationItemDelegate(myDelegate);
 *     iface->setAnnotationUniformItemSizes(true);
 * } else {
 *     // the implementation does not support the interface
 * }
 * \endcode
 *
 *  Porting from KF5 to KF6:
 *
 *  The subclass AnnotationViewInterfaceV2 was merged into its baseclass
 *  AnnotationViewInterface.
 *
 * \since 4.1
 */
class KTEXTEDITOR_EXPORT AnnotationViewInterface : public AnnotationInterface
{
public:
    ~AnnotationViewInterface() override
    {
    }

    /**
     * This function can be used to show or hide the annotation border
     * The annotation border is hidden by default.
     *
     * @param visible if \e true the annotation border is shown, otherwise hidden
     */
    virtual void setAnnotationBorderVisible(bool visible) = 0;

    /**
     * Checks whether the View's annotation border is visible.
     */
    virtual bool isAnnotationBorderVisible() const = 0;

    //
    // SIGNALS!!!
    //
public:
    /**
     * This signal is emitted before a context menu is shown on the annotation
     * border for the given line and view.
     *
     * \note Kate Part implementation detail: In Kate Part, the menu has an
     *       entry to hide the annotation border.
     *
     * \param view the view that the annotation border belongs to
     * \param menu the context menu that will be shown
     * \param line the annotated line for which the context menu is shown
     */
    virtual void annotationContextMenuAboutToShow(KTextEditor::View *view, QMenu *menu, int line) = 0;

    /**
     * This signal is emitted when an entry on the annotation border was activated,
     * for example by clicking or double-clicking it. This follows the KDE wide
     * setting for activation via click or double-clcik
     *
     * \param view the view to which the activated border belongs to
     * \param line the document line that the activated position belongs to
     */
    virtual void annotationActivated(KTextEditor::View *view, int line) = 0;

    /**
     * This signal is emitted when the annotation border is shown or hidden.
     *
     * \param view the view to which the border belongs to
     * \param visible the current visibility state
     */
    virtual void annotationBorderVisibilityChanged(KTextEditor::View *view, bool visible) = 0;

    /**
     * Sets the AbstractAnnotationItemDelegate for this view and the model
     * to provide custom rendering of annotation information for each line.
     * Ownership is not transferred.
     *
     * \param delegate the new AbstractAnnotationItemDelegate, or \c nullptr to reset to the default delegate
     *
     * @since 6.0
     */
    virtual void setAnnotationItemDelegate(KTextEditor::AbstractAnnotationItemDelegate *delegate) = 0;

    /**
     * Returns the currently used AbstractAnnotationItemDelegate
     *
     * @returns the current AbstractAnnotationItemDelegate
     *
     * @since 6.0
     */
    virtual KTextEditor::AbstractAnnotationItemDelegate *annotationItemDelegate() const = 0;

    /**
     * This function can be used to declare whether it is known that the annotation items
     * rendered by the set delegate all have the same size.
     * This enables the view to do some optimizations for performance purposes.
     *
     * By default the value of this property is \c false .
     *
     * @param uniformItemSizes if \c true the annotation items are considered to all have the same size
     *
     * @since 6.0
     */
    virtual void setAnnotationUniformItemSizes(bool uniformItemSizes) = 0;

    /**
     * Checks whether the annotation items all have the same size.
     *
     * @since 6.0
     */
    virtual bool uniformAnnotationItemSizes() const = 0;
};

}

Q_DECLARE_INTERFACE(KTextEditor::AnnotationInterface, "org.kde.KTextEditor.AnnotationInterface")
Q_DECLARE_INTERFACE(KTextEditor::AnnotationViewInterface, "org.kde.KTextEditor.AnnotationViewInterface")

#endif

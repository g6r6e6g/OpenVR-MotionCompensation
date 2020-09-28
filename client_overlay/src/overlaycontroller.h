#pragma once

#include <openvr.h>
#include <QtCore/QtCore>
#include <QObject>
#include <QSettings>
#include <QQmlEngine>
#include <QtGui/QOpenGLContext>
#include <QOffscreenSurface>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QQuickItem>
#include <QQuickRenderControl>
#include <QSoundEffect>
#include <memory>
#include <atomic>
#include "logging.h"

#include "tabcontrollers/DeviceManipulationTabController.h"

// application namespace
namespace motioncompensation
{
	class OverlayController : public QObject
	{
		Q_OBJECT
			Q_PROPERTY(bool desktopMode READ isDesktopMode)

	public:
		static constexpr const char* applicationKey = "ovrmc.VRMotionCompensation";
		static constexpr const char* applicationName = "OpenVR Motion Compensation";
		static constexpr const char* applicationVersionString = "v0.3.3";

	private:
		vr::VROverlayHandle_t m_ulOverlayHandle = vr::k_ulOverlayHandleInvalid;
		vr::VROverlayHandle_t m_ulOverlayThumbnailHandle = vr::k_ulOverlayHandleInvalid;

		std::unique_ptr<QQuickRenderControl> m_pRenderControl;
		std::unique_ptr<QQuickWindow> m_pWindow;
		std::unique_ptr<QOpenGLFramebufferObject> m_pFbo;
		std::unique_ptr<QOpenGLContext> m_pOpenGLContext;
		std::unique_ptr<QOffscreenSurface> m_pOffscreenSurface;

		std::unique_ptr<QTimer> m_pPumpEventsTimer;
		std::unique_ptr<QTimer> m_pRenderTimer;
		bool dashboardVisible = false;

		QPoint m_ptLastMouse;
		Qt::MouseButtons m_lastMouseButtons = 0;

		vrmotioncompensation::VRMotionCompensation m_vrMotionCompensation;

		bool desktopMode;
		bool noSound;

		QUrl m_runtimePathUrl;

		std::atomic_uint32_t m_uniqueNumber = 0;

		QSoundEffect activationSoundEffect;
		QSoundEffect focusChangedSoundEffect;

	public: // I know it's an ugly hack to make them public to enable external access, but I am too lazy to implement getters.
		DeviceManipulationTabController deviceManipulationTabController;

	private:
		OverlayController(bool desktopMode, bool noSound) : QObject(), desktopMode(desktopMode), noSound(noSound)
		{
		}

	public:
		virtual ~OverlayController();

		void Init(QQmlEngine* qmlEngine);
		void Shutdown();

		vrmotioncompensation::VRMotionCompensation& vrMotionCompensation()
		{
			return m_vrMotionCompensation;
		}

		bool isDashboardVisible()
		{
			return dashboardVisible;
		}

		void SetWidget(QQuickItem* quickItem, const std::string& name, const std::string& key = "");

		bool isDesktopMode()
		{
			return desktopMode;
		};

		Q_INVOKABLE QString getVersionString();
		Q_INVOKABLE QUrl getVRRuntimePathUrl();
		Q_INVOKABLE bool soundDisabled();

		const vr::VROverlayHandle_t& overlayHandle();
		const vr::VROverlayHandle_t& overlayThumbnailHandle();

	public slots:
		void renderOverlay();
		void OnRenderRequest();
		void OnTimeoutPumpEvents();

		void showKeyboard(QString existingText, unsigned long userValue = 0);

		void playActivationSound();
		void playFocusChangedSound();

	signals:
		void keyBoardInputSignal(QString input, unsigned long userValue = 0);

	private:
		static QSettings* _appSettings;
		static std::unique_ptr<OverlayController> singleton;

	public:
		static OverlayController* getInstance()
		{
			return singleton.get();
		}

		static OverlayController* createInstance(bool desktopMode, bool noSound)
		{
			singleton.reset(new motioncompensation::OverlayController(desktopMode, noSound));
			return singleton.get();
		}

		static QSettings* appSettings()
		{
			return _appSettings;
		}

		static void setAppSettings(QSettings* settings)
		{
			_appSettings = settings;
		}
	};

} // namespace motioncompensation
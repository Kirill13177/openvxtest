//@file demo_ViBe_Motion_Detection.h
//@brief Contains demonstration of ViBe Motion Detection
//@author Kirill Baykov
//@date 7 December 2019

#include "../stdafx.h"

#include <opencv2/opencv.hpp>

extern "C"
{
#include <Kernels/ref.h>
#include <types.h>
}

#include "../DemoEngine.h"

///////////////////////////////////////////////////////////////////////////////
//@brief Demonstration of Threshold function
class demo_ViBe_Motion_Detection : public IDemoCase
{
public:
    ///@brief default ctor
    demo_ViBe_Motion_Detection()
        : N(127)
    {
        // nothing to do
    }

    ///@see IDemoCase::ReplyName
    virtual std::string ReplyName() const override
    {
        return "ViBe Motion Detection";
    }

private:
    ///@see IDemoCase::execute
    virtual void execute() override;

    ///@brief provide interactive demo
    static void applyParameters(int pos, void* data);

private:
    cv::Mat m_srcImage;
    cv::Mat*** samples;
    int N;
    int R;
    int h_min;
    int phi;
};

///////////////////////////////////////////////////////////////////////////////
namespace
{
    const std::string m_openVXWindow = "openVX";
    const std::string m_openCVWindow = "openCV";
    const std::string m_originalWindow = "original";
    const std::string m_diffWindow = "Diff of " + m_openVXWindow + " and " + m_openCVWindow;
}

///////////////////////////////////////////////////////////////////////////////
void ViBe_Motion_Detection::execute()
{
    cv::namedWindow(m_originalWindow, CV_WINDOW_NORMAL);
    cv::namedWindow(m_openVXWindow, CV_WINDOW_NORMAL);
    cv::namedWindow(m_openCVWindow, CV_WINDOW_NORMAL);
    cv::namedWindow(m_diffWindow, CV_WINDOW_NORMAL);

    const std::string imgPath = "../Image/Solvay_conference_1927.png";
    m_srcImage = cv::imread(imgPath, CV_LOAD_IMAGE_GRAYSCALE);
    cv::imshow(m_originalWindow, m_srcImage);

    cv::createTrackbar("ViBe_Motion_Detection:", m_originalWindow, &m_N, &m_R, &m_h_min, &m_phi, 150, 1, 3, 0.125, applyParameters, static_cast<void*>(this));
    applyParameters(m_N, m_R, m_h_min, m_phi, this);

    cv::waitKey(0);
}

///////////////////////////////////////////////////////////////////////////////
void ViBe_Motion_Detection::applyParameters(int, void* data)
{
    auto demo = static_cast<demo_ViBeMotionDetection*>(data);

    const cv::Size imgSize(demo->m_srcImage.cols, demo->m_srcImage.rows);

    ///@{ OPENCV
    cv::Mat cvImage;
    cv::N(demo->m_srcImage, cvImage, demo->m_N, 150, CV_VIBEMOTIONDETECTION_BINARY);
    cv::R(demo->m_srcImage, cvImage, demo->m_R, 1, CV_VIBEMOTIONDETECTION_BINARY);
    cv::h_min(demo->m_srcImage, cvImage, demo->m_h_min, 3, CV_VIBEMOTIONDETECTION_BINARY);
    cv::phi(demo->m_srcImage, cvImage, demo->m_phi, 0.125, CV_VIBEMOTIONDETECTION_BINARY);
    cv::imshow(m_openCVWindow, cvImage);
    ///@}

    ///@{ OPENVX
    _vx_ViBeMotionDetection vxN = { VX_VIBEMOTIONDETECTION_TYPE_BINARY, uint8_t(demo->m_N), 0/* dummy value */, 255 /* dummy value */ };
    _vx_ViBeMotionDetection vxR = { VX_VIBEMOTIONDETECTION_TYPE_BINARY, uint8_t(demo->m_R), 0/* dummy value */, 255 /* dummy value */ };
    _vx_ViBeMotionDetection vxh_min = { VX_VIBEMOTIONDETECTION_TYPE_BINARY, uint8_t(demo->m_h_min), 0/* dummy value */, 255 /* dummy value */ };
    _vx_ViBeMotionDetection vxphi = { VX_VIBEMOTIONDETECTION_TYPE_BINARY, uint8_t(demo->m_phi), 0/* dummy value */, 1 /* dummy value */ };
    _vx_image srcVXImage = {
       demo->m_srcImage.data,
       uint32_t(imgSize.width),
       uint32_t(imgSize.height),
       VX_DF_IMAGE_U8,
       VX_COLOR_SPACE_DEFAULT
    };

    uint8_t* outVXImage = static_cast<uint8_t*>(calloc(imgSize.width * imgSize.height, sizeof(uint8_t)));
    _vx_image dstVXImage = {
       outVXImage,
       uint32_t(imgSize.width),
       uint32_t(imgSize.height),
       VX_DF_IMAGE_U8,
       VX_COLOR_SPACE_DEFAULT
    };

    ref_ViBe_Motion_Detection(&srcVXImage, &dstVXImage, &vxN, &vxR, &vxh_min, &vxphi);

    const cv::Mat vxImage = cv::Mat(imgSize, CV_8UC1, outVXImage);
    cv::imshow(m_openVXWindow, vxImage);
    ///@}

    // Show difference of OpenVX and OpenCV
    const cv::Mat diffImage(imgSize, CV_8UC1);
    cv::absdiff(vxImage, cvImage, diffImage);
    cv::imshow(m_diffWindow, diffImage);
}

///////////////////////////////////////////////////////////////////////////////
IDemoCasePtr CreateViBeMotionDetectingDemo()
{
    return std::make_unique<demo_ViBe_Motion_Detection>();
}

#include <opencv2/opencv.hpp>
#include <iostream>

// 自作二値化関数
void my_threshold
(
const cv::Mat& src, cv::Mat& dst,
double thresh, double max_value
)
{
	int x, y = 0;
	unsigned char intensity = 0;
	for (y = 0; y < src.rows; y++)
	{
		for (x = 0; x < src.cols; x++)
		{
			intensity = src.at<unsigned char>(y, x);
			if (intensity < (unsigned char)thresh)
			{
				dst.at<unsigned char>(y, x) = 0;
			}
			else
			{
				dst.at<unsigned char>(y, x) = (unsigned char)max_value;
			}
		}
	}
}

// cv::parallel_for_利用コード
class TestParallelLoopBody : public cv::ParallelLoopBody
{
private:
	cv::Mat _src;
	cv::Mat _dst;
	double _thresh;
	double _max_value;
public:
	TestParallelLoopBody
		(
		const cv::Mat& src, cv::Mat& dst,
		double thresh, double max_value
		)
		: _src(src), _dst(dst), _thresh(thresh), _max_value(max_value) { }
	void operator() (const cv::Range& range) const
	{
		int row0 = range.start;
		int row1 = range.end;
		cv::Mat srcStripe = _src.rowRange(row0, row1);
		cv::Mat dstStripe = _dst.rowRange(row0, row1);
		my_threshold(srcStripe, dstStripe, _thresh, _max_value);
	}
};

int main(int argc, char *argv[])
{
	cv::Mat src(cv::Size(5000, 5000), CV_8UC1, cv::Scalar(255));
	cv::Mat dst = cv::Mat::zeros(src.size(), src.type());
	double f = 1000.0f / cv::getTickFrequency();

	int64 start = cv::getTickCount();

	// 自作二値化関数の実行
	my_threshold(src, dst, 100, 255);

	int64 end = cv::getTickCount();
	std::cout << "my_threshold: " <<
		(end - start) * f << "[ms]" << std::endl;

	start = cv::getTickCount();

	// cv::parallel_for_を使って自作二値化関数を並列化
	cv::parallel_for_
		(
		cv::Range(0, dst.rows),
		TestParallelLoopBody(src, dst, 100, 255)
		);

	end = cv::getTickCount();
	std::cout << "my_threshold(parallel_for_): "
		<< (end - start) * f << "[ms]" << std::endl;
	getchar();
	return 0;
}
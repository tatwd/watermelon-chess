#include "Board.h"
#include "ui_Board.h"

#include <QImage>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QSound>
#include <math.h>

#include "AboutDlg.h"
#include "HelpDlg.h"
#include "ModelDlg.h"
#include "ResignDlg.h"

#define GetAR(__angle, __r, __id)                                              \
	int __angle = _s[__id]._polarAngle;                                        \
	int __r = _s[__id]._polarDiameter

Board::Board(QWidget *parent) : QMainWindow(parent), ui(new Ui::Board)
{
	ui->setupUi(this);

	// this->setMouseTracking(true);  //设置为不按下鼠标键触发moveEvent

	_R = 180; //棋盘外圆半径
	init();	  //初始化棋子

	// ui->mainToolBar->close(); //默认关闭工具栏
	ui->statusBar->showMessage(tr("按F1查看规则"));
}

Board::~Board() { delete ui; }

//--------------------------------------------绘制棋盘及棋子-------------------------------------------------//

//初始化棋子
void Board::init()
{
	_selectId = -1;
	_movedId = -1;
	_killId = -1;
	_beBlackTurn = true; //黑棋先走

	_endGame = false;

	for (int i = 0; i < 12; ++i) {
		_s[i].init(i); //初始化棋子
	}
}

//绘图事件
void Board::paintEvent(QPaintEvent *)
{
	//------------------------笔刷屏幕------------------------------//
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing); //绘图抗锯齿

	// painter.setWindow(QRect(-350,250,700,500));

	painter.fillRect(rect(), Qt::white); //界面填充为白色

	double cx = width() / 2;
	double cy = height() / 2;

	painter.translate(cx, cy); //将屏幕中心设为(0,0)

	QPen pen(Qt::darkYellow, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	painter.setPen(pen);

	//------------------------图片部分------------------------------//
	QImage img;
	// img.load("assets/img/bg3.jpg"); //加载背景图片
	painter.drawImage(QPoint(-cx, -cy), img);

	//------------------------画圆部分------------------------------//
	QPoint ct(0, 0); //圆心

	for (int i = 0; i < 2; ++i) {
		int i_r = _R;
		if (i == 1)
			i_r = _R / 3;
		painter.drawEllipse(ct, i_r, i_r); //画内外两圆
	}

	//------------------------画线部分------------------------------//
	for (int i = 0; i < 2; ++i) {
		QPoint l_start(ct.x() + _R * i, ct.y() + _R * (i - 1));
		QPoint l_end(ct.x() - _R * i, ct.y() - _R * (i - 1));

		painter.drawLine(l_start, l_end);
	}

	//------------------------画四条弧------------------------------//
	QPainterPath path;
	for (int i = 0; i < 4; ++i) {
		int i_r = _R;
		QPoint startPoint = polarCoordToXY(ct, 30 + 90 * i, i_r); //当前点
		QPoint endPoint = polarCoordToXY(ct, -30 + 90 * i, i_r);  //终止点
		QPoint ctrlPoint = polarCoordToXY(ct, 90 * i, i_r * 105 / 180); //控制点

		path.moveTo(startPoint);		  //移动到当前点
		path.quadTo(ctrlPoint, endPoint); //设置二次贝塞尔曲线
		painter.drawPath(path);			  //画二次贝塞尔曲线
	}

	//------------------------绘制棋子------------------------------//
	for (int i = 0; i < 12; ++i) {
		if (!isDead(i)) { //判断是否是死子
			drawStone(painter, ct, i);
		} else {
			Step *step = _steps.last(); // bug

			//判断此死子是否在_steps中
			if (!isKillIdOfStep(i, _steps)) {
				_killId = i;
				step->_killId = _killId;
			}
			_killId = -1;

			// QSound::play("assets/sound/deadstoneless.wav");
		}
	}
}

/*极坐标转化成棋子的像素坐标*/
QPoint Board::polarCoordToXY(QPoint c, double angle, double r)
{
	QPoint pos;

	double _ang = angle * M_PI / 180; //角度转弧度
	// double _ang = ang*16;

	pos.rx() = r * cos(_ang) + c.x();
	pos.ry() = c.y() - r * sin(_ang);

	return pos;
}

QPoint Board::polarCoordToXY(QPoint c, int id)
{
	return polarCoordToXY(c, _s[id]._polarAngle, _s[id]._polarDiameter);
}

/*画棋子模块，在paintEvent()中调用*/
void Board::drawStone(QPainter &painter, QPoint &c, int id)
{
	int _r = 14;						 //棋子半径
	QPoint s_ct = polarCoordToXY(c, id); //获取棋子的像素坐标

	/*设置棋子颜色*/
	painter.setPen(Qt::white);
	QPoint rp;
	rp.rx() = s_ct.x() + _r;
	rp.ry() = s_ct.y() - _r;
	QRadialGradient rg; //辐射渐变
	if (_s[id]._black) {
		QRadialGradient black_s(s_ct, _r, rp);
		rg = black_s;
		rg.setColorAt(0, Qt::gray);

	} else {
		QRadialGradient white_s(rp, 4 * _r, s_ct);
		rg = white_s;
		rg.setColorAt(0, Qt::white);
	}
	rg.setColorAt(1, Qt::black);
	painter.setBrush(rg);

	painter.drawEllipse(s_ct, _r, _r);

	/*选中棋子效果*/
	if (id == _selectId) {
		painter.setPen(Qt::red);
		painter.drawEllipse(s_ct, _r / 2, _r / 2);
	}

	/*被走棋子效果*/
	if (id == _movedId) {
		painter.setPen(Qt::red);
		painter.drawEllipse(s_ct, _r / 2, _r / 2);
	}
}

/*遍历棋盘所以的点并返回其像素坐标*/
QPoint Board::getPointOfBoard(int pos, int &angle, int &r)
{
	QPoint bCenter(0, 0); //棋盘中心坐标

	getAROfBoard(pos, angle, r); //获取该点的极坐标

	return polarCoordToXY(bCenter, angle, r);
}

/*获取棋盘上点的极坐标*/
void Board::getAROfBoard(int pos, int &angle, int &r)
{
	angle = 0;
	r = 0;

	if (pos >= 0 && pos <= 11) { // 0~11第1圈
		angle = pos * 30;
		r = _R;
	} else if (pos >= 12 && pos <= 15) { // 12~15第2圈
		angle = (pos % 4) * 90;
		r = _R * 13 / 18;
	} else if (pos >= 16 && pos <= 19) { // 16~19第3圈
		angle = (pos % 4) * 90;
		r = _R / 3;
	}
}

//---------------------------------------------基本走棋-----------------------------------------------------//

/*将变换后的坐标还原成像素坐标*/
void Board::transpCoordToXY(QPoint &pos)
{
	pos.rx() -= width() / 2;
	pos.ry() -= height() / 2;
	// return pos;
}

/*鼠标释放事件*/
void Board::mouseReleaseEvent(QMouseEvent *ev)
{
	if (ev->button() != Qt::LeftButton) //鼠标左右键判断
		return;
	click(ev->pos()); //获取点击位置坐标
}

/*点击棋盘，获取位置及棋子id*/
void Board::click(QPoint pt)
{
	//将坐标还原成像素坐标
	transpCoordToXY(pt);

	//将pt转化成棋子的极角、极径,然后判断此极点上是否有棋子

	int angle = 0; //记录点击位置的极角
	int r = 0;	   //记录点击位置的极径

	bool flag = getPolarCoord(pt, angle, r);
	if (flag == false)
		return;

	int clickId = getStoneId(angle, r);

	click(clickId, angle, r);
}
void Board::click(int id, int angle, int r)
{
	if (this->_selectId == -1) { //棋盘无被选棋子
		trySelectStone(id);
	} else {
		tryMoveStone(id, angle, r);
		getResult(); //判断输赢
	}
}

/*获取被点棋子的id*/
int Board::getStoneId(int angle, int r)
{
	for (int i = 0; i < 12; ++i) {
		if (_s[i]._polarAngle == angle && _s[i]._polarDiameter == r &&
			!isDead(i))
			return i;
	}
	return -1;
}

/*判断棋子能否被选*/
bool Board::canSelect(int id) { return _beBlackTurn == _s[id]._black; }

/*选中棋子*/
void Board::trySelectStone(int id)
{
	if (id == -1) //未点中棋子
		return;

	if (canSelect(id)) {
		_selectId = id;
		_movedId = -1;
		update();
	}
}

/*尝试走棋*/
void Board::tryMoveStone(int id, int angle, int r)
{
	if (_selectId == id) { //同颗棋子被点2次时
		_selectId = -1;
		_movedId = -1;
		update();
	} else {
		if (id != -1) { //点中别的棋子时
			id = -1;
			return;
		}
		bool go = canMove(_selectId, angle, r); //判断目标位置是否可走棋
		if (!go)
			return;

		playMoveSound();						 //播放走棋声音
		moveStone(_selectId, _killId, angle, r); //走棋
		movePromptInStatusBar();				 //走棋提示
		update();
	}
}

/*走棋*/
void Board::moveStone(int movedId, int killId, int angle, int r)
{
	saveStep(movedId, killId, angle, r, _steps);
	moveStone(movedId, angle, r);
}

void Board::moveStone(int movedId, int angle, int r)
{
	//将目标位置的极坐标赋给被选棋子
	_s[movedId]._polarAngle = angle;
	_s[movedId]._polarDiameter = r;
	_movedId = movedId;
	_selectId = -1;

	setBeBlackTurn(); //下次轮反色棋子走
}

/*播放走棋声音*/
void Board::playMoveSound() { QSound::play("assets/sound/move.wav"); }

/*下棋提示，在状态栏显示*/
void Board::movePromptInStatusBar()
{
	if (!_beBlackTurn)
		ui->statusBar->showMessage("下一步：白方");
	else
		ui->statusBar->showMessage("下一步：黑方");
}

/*像素坐标转成棋子的极点坐标并判断是否有子，待优化*/
bool Board::getPolarCoord(QPoint pt, int &angle, int &r)
{
	int _r = 14;

	for (int i = 0; i < 21; ++i) {
		QPoint c = getPointOfBoard(i, angle, r);

		int dx = c.x() - pt.x();
		int dy = c.y() - pt.y();
		if ((dx * dx + dy * dy) < _r * _r) {
			return true;
		}
	}
	return false;
}
bool Board::getPolarCoord(QPoint pt)
{
	int angle = 0;
	int r = 0;

	bool inBoardPoint = getPolarCoord(pt, angle, r);
	if (inBoardPoint) {
		int id = getStoneId(angle, r);
		return canMove(id, angle, r);
	}
	return false;
}

/*保存步数*/
void Board::saveStep(int movedId, int killId, int angle, int r,
					 QVector<Step *> &steps)
{
	GetAR(iangle, ir, movedId);

	Step *step = new Step;

	step->_movedId = movedId;
	step->_killId = killId;
	step->_angleFrom = iangle;
	step->_rFrom = ir;
	step->_angleTo = angle;
	step->_rTo = r;

	steps.append(step);
	// delete  step;
}

//---------------------------------------------规则部分-----------------------------------------------------//

/*获取点击位置的圈数*/
int Board::getLaps(int r)
{
	const int R = 180; //记录棋盘半径
	int laps = 0;
	switch (r) {
	case R:
		laps = 0;
		break;
	case R * 13 / 18:
		laps = 1;
		break;
	case R / 3:
		laps = 2;
		break;
	case 0:
		laps = 3;
		break;
	}
	return laps;
}

/*判断目标位置是否能走棋*/
bool Board::canMove(int id, int eAngle, int eR)
{
	/* 棋子移动时：
	 * 极角变化为：0度 30度 90度
	 * 极径变化为：0 _R/3 _R*7/18 _R*5/18
	 * 共计12种组合,但实际存在的只有6种
	 */
	struct {
		int d_angle;
		int d_r;
	} d_pos[6] = {
		{30, 0},							 // 1<->1
		{0, _R * 5 / 18}, {30, _R * 5 / 18}, // 1<->2
		{0, _R * 7 / 18},					 // 2<->3
		{90, 0},							 // 3<->3
		{0, _R / 3},						 // 3<->4
	};

	int dAngle = abs(eAngle - _s[id]._polarAngle); //极角变化
	int dR = abs(eR - _s[id]._polarDiameter);	   //极径变化
	int labs = getLaps(_s[id]._polarDiameter);	   //棋子所在圈数

	if ((dAngle == 90 || dAngle == 270 || dAngle == 180) && (dR == _R / 3))
		dAngle = 0;
	else if (dAngle == 270 && dR == 0 && eR == _R / 3 && labs == 2)
		dAngle = 90;
	else if (dAngle == 330)
		dAngle = 30;

	bool canGo = false;
	for (int i = 0; i < 6; ++i) {
		if (i == 4 && (labs == 0 || labs == 1)) //棋子在第1,2圈时，排除第{90，0}
			continue;

		if (d_pos[i].d_angle == dAngle && d_pos[i].d_r == dR) {
			canGo = true;
			break;
		}
	}

	return canGo;
}

/*判断死子，true为死子*/

bool Board::isDead(int id)
{
	QSet<QString> visited;
	return isDead2(id, visited);
}

bool Board::isDead2(int id, QSet<QString> visited)
{
    if (_s[id]._dead)
        return true;

	// 先获取周围的点然后遍历递归判断死活
	QVector<BoardPoint> aroundPoints = getSurroundPoints(id);

	if (aroundPoints.isEmpty()) {
		return false;
	}

	for (auto item : aroundPoints) {

		QString str = item.GetString();
		if (visited.contains(str)) // 该点已被遍历过了
			continue;

		visited.insert(str);

		Stone *pStone = getStone(item);

		if (nullptr == pStone) { // 没棋子
			_s[id]._dead = false;
			return false;
		}

        if (pStone->_dead) { // 周围棋子已死
            _s[id]._dead = false;
            return false;
        }

		if (pStone->_black == _s[id]._black) { // 同颜色棋子

			if (!isDead2(pStone->_id, visited)) { // 没死
				_s[id]._dead = false;
				return false;
			}
		}
	}

	_s[id]._dead = true;
	return _s[id]._dead;
}

// 获取周围的点 maybe move to Stone.cpp
QVector<BoardPoint> Board::getSurroundPoints(int id)
{
	Stone cur = _s[id];

	int angle = cur._polarAngle;
	int diameter = cur._polarDiameter;

	QVector<BoardPoint> vec;

	int r1 = _R;
	int r2 = _R * 13 / 18;
	int r3 = _R / 3;

	// TODO
	if (0 == diameter && 0 == angle) { // 原点
		// 4 个点
		vec.push_back(BoardPoint(0, r3));
		vec.push_back(BoardPoint(90, r3));
		vec.push_back(BoardPoint(180, r3));
		vec.push_back(BoardPoint(270, r3));
	} else if (r3 == diameter) { // 第3圈
		vec.push_back(BoardPoint(angle, r2));
		vec.push_back(BoardPoint(angle == 270 ? 0 : angle + 90, r3));
		vec.push_back(BoardPoint(0, 0));
		vec.push_back(BoardPoint(angle == 0 ? 270 : angle - 90, r3));
	} else if (r2 == diameter) { // 第2圈
		vec.push_back(BoardPoint(angle, r3));
		vec.push_back(BoardPoint(angle + 30, r1));
		vec.push_back(BoardPoint(angle, r1));
		vec.push_back(BoardPoint(angle == 0 ? 330 : angle - 30, r1));
	} else if (r1 == diameter && (angle % 90) == 0) { // 第1圈 中间点
		vec.push_back(BoardPoint(angle, r2));
		vec.push_back(BoardPoint(angle + 30, r1));
		vec.push_back(BoardPoint(angle == 0 ? 330 : angle - 30, r1));
	} else if (r1 == diameter && (30 == angle || 120 == angle || 210 == angle ||
								  300 == angle)) { // 第1圈 下面1点
		vec.push_back(BoardPoint(angle + 30, r1));
		vec.push_back(BoardPoint(angle - 30, r1));
		vec.push_back(BoardPoint(angle - 30, r2));
	} else if (r1 == diameter && (60 == angle || 150 == angle || 240 == angle ||
								  330 == angle)) { // 第1圈 上面1点
		vec.push_back(BoardPoint(angle - 30, r1));
		vec.push_back(BoardPoint(angle == 330 ? 0 : angle + 30, r1));
		vec.push_back(BoardPoint(angle == 330 ? 0 : angle + 30, r2));
	}

	return vec;
}

/*获取棋子被围数*/
int Board::getSurroundStone(int id)
{
	QPoint sCenter = polarCoordToXY(QPoint(0, 0), id);

	int d = _R * 10 / 18;
	int n = 0;
	for (int i = 0; i < 12; ++i) {
		if (_s[i]._dead)
			continue;

		QPoint iCenter = polarCoordToXY(QPoint(0, 0), i);

		int dx = sCenter.x() - iCenter.x();
		int dy = sCenter.y() - iCenter.y();

		if ((dx * dx + dy * dy) > 0 && (dx * dx + dy * dy) <= d * d &&
			_s[i]._black != _s[id]._black)
			n++;
	}
	return n;
}

/*判断位置是否有子*/
bool Board::hasStone(int angle, int r)
{
	for (int i = 0; i < 12; ++i) {
		if (_s[i]._polarAngle == angle && _s[i]._polarDiameter == r)
			return true;
	}
	return false;
}

Stone *Board::getStone(BoardPoint point)
{
	for (int i = 0; i < 12; ++i) {
		if (_s[i]._polarAngle == point.PolarAngle &&
			_s[i]._polarDiameter == point.PolarDiameter)
			return &_s[i];
	}

	return nullptr;
}

/*获取单个棋子的气*/
int Board::getLiberty(int id)
{
	// int d = _R*10/18;
	int liberty = 0; //气数
	int labs = getLaps(id);
	int surroundStone = getSurroundStone(id); //未过滤全部被同色棋子包围情况

	if (surroundStone == 0 && (labs == 3 || labs == 2 || labs == 1))
		liberty = 4 - surroundStone;
	else if (surroundStone == 0 && labs == 0)
		liberty = 3 - surroundStone;

	//    QPoint sPoint = polarCoordToXY(QPoint(0, 0), id);
	//    for(int i = 0; i < 21; ++i)
	//    {
	//        if(_s[i]._dead)
	//            continue;
	//        int iangle = 0;
	//        int ir = 0;
	//        QPoint c = getPointOfBoard(i, iangle, ir);
	//        int dx = c.x() - sPoint.x();
	//        int dy = c.y() - sPoint.y();
	//        if((dx*dx + dy*dy)>0 && (dx*dx + dy*dy) <= d*d)
	//        {
	//            if(hasStone(iangle, ir))
	//            {
	//                //if(i == id)
	//                //{
	//                    if(_s[i]._black  == _s[id]._black) //同色棋子相邻时
	//                        liberty += getLiberty(i);
	//                    else
	//                        liberty -= 1;
	//                //}
	//            }
	//            else
	//            {
	//                liberty += 1;
	//            }
	//        }
	//    }

	return liberty;
}
// new add
int Board::getLiberty(int preid, int nextid, QVector<int> &stoneid)
{
	int d = _R * 10 / 18;
	int liberty = 0; //气数

	stoneid.append(nextid);

	QPoint sPoint = polarCoordToXY(QPoint(0, 0), nextid);
	for (int i = 0; i < 21; ++i) {
		if (_s[i]._dead)
			continue;

		int iangle = 0;
		int ir = 0;
		QPoint c = getPointOfBoard(i, iangle, ir);
		int dx = c.x() - sPoint.x();
		int dy = c.y() - sPoint.y();

		if ((dx * dx + dy * dy) > 0 && (dx * dx + dy * dy) <= d * d) {
			if (hasStone(iangle, ir)) {
				if (!inNextid(i, &stoneid)) {
					if (_s[i]._black == _s[nextid]._black) //同色棋子相邻时
						liberty += getLiberty(preid, i, stoneid);
					else
						liberty -= 1;
				}

			} else {
				// nextid = -1;
				liberty += 1;
			}
		}
	}

	return liberty;
}
// new add
bool Board::inNextid(int id, QVector<int> *stoneid)
{
	for (QVector<int>::iterator it = stoneid->begin(); it != stoneid->end();
		 ++it) {
		if (id == *it)
			return true;
	}
	return false;
}

/*获取棋子数*/
int Board::getNumOfStone(bool isblack)
{
	int numOfStone = 0;
	int startId = 0;

	if (!isblack) //当颜色不为黑
		startId = 6;

	for (int i = startId; i < startId + 6; ++i) {
		if (!_s[i]._dead)
			numOfStone++;
	}
	return numOfStone;
}

/*轮到谁走*/
void Board::setBeBlackTurn() { this->_beBlackTurn = !this->_beBlackTurn; }

/*判断输赢*/
void Board::getResult()
{
	bool isPeace = false;
	if (getNumOfStone(_beBlackTurn) < 3) //小于3颗即输
	{
		//弹出认输框
		ResignDlg resignDlg(this);
		resignDlg.changeText(_beBlackTurn, isPeace);
		QApplication::beep(); //警报声
		resignDlg.exec();

		_endGame = true;
	}
}

/*复活死子*/
void Board::reliveStone(int id)
{
	if (id == -1)
		return;
	_s[id]._dead = false;
}

//杀死棋子
void Board::killStone(int id)
{
	if (id == -1)
		return;
	_s[id]._dead = true;
}

/*悔棋*/
void Board::backStep() //单步悔棋
{
	if (this->_steps.size() == 0)
		return;

	Step *step = this->_steps.last(); //取被悔棋子的上一步
	_steps.removeLast();
	back(step);

	_selectId = step->_movedId; //使要悔棋子处于被选状态

	movePromptInStatusBar(); //走棋提示
	update();

	delete step;
}

/*判断死子是否已经被棋子杀死*/
bool Board::isKillIdOfStep(int id, QVector<Step *> steps)
{
	for (QVector<Step *>::iterator it = steps.begin(); it != steps.end();
		 ++it) {
		Step *step = *it;
		if (id == step->_killId) {
			// delete step;
			return true;
		}
		// delete step;
	}
	return false;
}

void Board::back(Step *step)
{
	reliveStone(step->_killId); //复活所杀棋子
	moveStone(step->_movedId, step->_angleFrom, step->_rFrom);
}

void Board::back() { backStep(); }

/*更改鼠标形状,未实现*/
void Board::mouseMoveEvent(QMouseEvent *ev)
{
	//    QPoint mp = ev->pos();

	//    transpCoordToXY(mp);

	//    bool inStone = getPolarCoord(mp);
	//    if(inStone)
	//    {
	//        this->setCursor(Qt::PointingHandCursor);
	//    }
	//    else
	//    {
	//        this->setCursor(Qt::ArrowCursor);      //范围之外变回原来形状
	//    }
}

/*设置鼠标成手型*/
void Board::setHandCursor() { setCursor(QCursor(Qt::PointingHandCursor)); }

//---------------------------------------------菜单栏-------------------------------------------------------//

/*对局*/
void Board::on_action_New_triggered() //新对局
{
	ModelDlg modelDlg(this);
	modelDlg.exec();
}

/*功能*/
void Board::on_action_Back_triggered() //悔棋
{
	back();
}

void Board::on_action_Peace_triggered() //和棋
{
	bool isPeace = true;
	ResignDlg rd(this);
	rd.changeText(_beBlackTurn, isPeace);
	QApplication::beep(); //警报声
	rd.exec();
}

void Board::on_action_Resign_triggered() //认输
{
	bool isPeace = false;
	ResignDlg rd(this);
	rd.changeText(_beBlackTurn, isPeace);
	QApplication::beep(); //警报声
	rd.exec();
}

void Board::on_action_Flush_triggered() //刷新
{
	init();
	movePromptInStatusBar();
	update();
}

/*视图*/
void Board::on_action_DisplayToolBar_triggered() //显示工具栏
{
	if (ui->action_DisplayToolBar->text() == "显示工具栏") {
		ui->mainToolBar->show();
		ui->action_DisplayToolBar->setText("关闭工具栏");
	} else {
		ui->mainToolBar->close();
		ui->action_DisplayToolBar->setText("显示工具栏");
	}
}

void Board::on_action_CloseMenuBar_triggered() //关闭菜单栏
{
	if (ui->action_CloseMenuBar->text() == "关闭菜单栏") {
		ui->menuBar->close();
		ui->action_CloseMenuBar->setText("显示菜单栏");
	} else {
		ui->menuBar->show();
		ui->action_CloseMenuBar->setText("显示菜单栏");
	}
}

/*帮助*/
void Board::on_action_About_triggered() //关于
{
	AboutDlg aboutDlg(this);
	aboutDlg.exec();
}

void Board::on_action_Help_triggered() //玩法
{
	HelpDlg helpDlg(this);
	helpDlg.exec();
}

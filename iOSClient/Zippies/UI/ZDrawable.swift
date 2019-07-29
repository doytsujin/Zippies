
import Foundation
import UIKit

let TURN_RADIUS = 30.0

protocol ZDrawable
{
    
    func draw(_ transform: CGAffineTransform)
    
}

class ZCompoundDrawable: ZDrawable
{

    fileprivate let drawables: [ZDrawable]
    
    init(_ drawables: ZDrawable...)
    {
        self.drawables = drawables
    }
    
    func draw(_ transform: CGAffineTransform)
    {
        for nextDrawable in drawables {
            nextDrawable.draw(transform)
        }
    }
    
}

class ZDrawableLine: ZDrawable
{
    fileprivate let color: UIColor
    fileprivate let start: CGPoint
    fileprivate let end: CGPoint
    
    init(
        _ color: UIColor,
        _ startX: Double,
        _ startY: Double,
        _ endX: Double,
        _ endY: Double)
    {
        self.color = color
        self.start = CGPoint(x: startX, y: startY)
        self.end = CGPoint(x: endX, y: endY)
    }
    
    convenience init(
        _ color: UIColor,
        _ start: KVector2,
        _ endX: Double,
        _ endY: Double)
    {
        self.init(color, start.getX(), start.getY(), endX, endY)
    }
    
    convenience init(
        _ color: UIColor,
        _ startX: Double,
        _ startY: Double,
        _ end: KVector2)
    {
        self.init(color, startX, startY, end.getX(), end.getY())
    }
    
    convenience init(
        _ color: UIColor,
        _ start: KVector2,
        _ end: KVector2)
    {
        self.init(color, start.getX(), start.getY(), end.getX(), end.getY())
    }
    
    func draw(_ transform: CGAffineTransform)
    {
        color.setStroke()
        let orientationIndicator = UIBezierPath()
        orientationIndicator.move(to: start)
        orientationIndicator.addLine(to: end)
        orientationIndicator.apply(transform)
        orientationIndicator.stroke()
    }
    
}

class ZDrawableArrow: ZDrawable
{
    fileprivate let color: UIColor
    fileprivate let position: CGPoint
    fileprivate let orientation: CGFloat
    fileprivate let size: CGFloat
    
    init(_ color: UIColor, _ x: Double, _ y: Double, _ o: Double, _ s: Double)
    {
        self.color = color
        self.position = CGPoint(x: x, y: y)
        self.orientation = CGFloat(o)
        self.size = CGFloat(s)
    }
    
    init(_ color: UIColor, _ p: KMatrix2, _ s: Double)
    {
        self.color = color
        self.position = CGPoint(x: p.position.getX(), y: p.position.getY())
        self.orientation = CGFloat(p.orientation.rotation)
        self.size = CGFloat(s)
    }
    
    func draw(_ transform: CGAffineTransform)
    {
        color.setStroke()
        let radiusSinO = size * sin(self.orientation)
        let radiusCosO = size * cos(self.orientation)
        let centerPoint = CGPoint(
            x: self.position.x - radiusSinO,
            y: self.position.y - radiusCosO)
        let orientationIndicator = UIBezierPath()
        
        orientationIndicator.move(
            to: CGPoint(
                x: centerPoint.x - radiusCosO,
                y: centerPoint.y + radiusSinO))
        orientationIndicator.addLine(to: position)
        orientationIndicator.addLine(
            to: CGPoint(
                x: centerPoint.x + radiusCosO,
                y: centerPoint.y - radiusSinO))
        orientationIndicator.apply(transform)
        orientationIndicator.stroke()
    }
    
}

class ZDrawableTurn: ZDrawable
{
    
    fileprivate let color: UIColor
    fileprivate let startPosition: KVector2
    fileprivate let turn: Turn
    
    init(_ color: UIColor, _ startX: Double, _ startY: Double, _ turn: Turn)
    {
        self.color = color
        self.startPosition = KVector2(startX, startY);
        self.turn = turn;
    }

    func draw(_ transform: CGAffineTransform)
    {
        color.setStroke()
        let drawingPath = UIBezierPath(
            ovalIn: CGRect(
                x: startPosition.getX() - TURN_RADIUS,
                y: startPosition.getY() - TURN_RADIUS,
                width: 2 * TURN_RADIUS,
                height: 2 * TURN_RADIUS))
        drawingPath.apply(transform)
        drawingPath.stroke()
    }
    
}

class ZDrawableCircle: ZDrawable
{
    
    fileprivate let color: UIColor
    fileprivate let center: CGPoint
    fileprivate let radius: CGFloat
    fileprivate let fill: Bool
    
    init(_ color: UIColor, _ x: Double, _ y: Double, _ r: Double, _ fill: Bool)
    {
        self.color = color
        self.center = CGPoint(x: x, y: y)
        self.radius = CGFloat(r)
        self.fill = fill
    }
    
    convenience init(_ color: UIColor, _ x: Double, _ y: Double, _ r: Double)
    {
        self.init(color, x, y, r, false)
    }
    
    convenience init(_ color: UIColor, _ center: KVector2, _ radius: Double)
    {
        self.init(color, center.getX(), center.getY(), radius, false)
    }
    
    convenience init(_ color: UIColor, _ center: KVector2, _ radius: Double, _ fill: Bool)
    {
        self.init(color, center.getX(), center.getY(), radius, fill)
    }
    
    func draw(_ transform: CGAffineTransform)
    {
        let drawingPath = UIBezierPath(
            ovalIn: CGRect(
                x: center.x - radius,
                y: center.y - radius,
                width: 2 * radius,
                height: 2 * radius))
            /*
            UIBezierPath(
            arcCenter: center,
            radius: radius,
            startAngle: 0,
            endAngle: 0,
            clockwise: true)
            */
        drawingPath.apply(transform)
        
        if fill {
            color.setFill()
            drawingPath.fill()
        }
        else {
            color.setStroke()
            drawingPath.stroke()
        }
    }
    
}

class ZDrawablePath: ZDrawable
{
    
    fileprivate let color: UIColor
    fileprivate let paths: [ZPath]
    
    init(_ color: UIColor, _ paths: ZPath...)
    {
        self.color = color
        self.paths = paths
    }
    
    func draw(_ transform: CGAffineTransform)
    {
        color.setStroke()

        //draw the path
        let nextPoint = KMatrix2()
        let drawingPath = UIBezierPath()
        for nextPath in paths {
            nextPath.interpolate(0, nextPoint)
            drawingPath.move(to: CGPoint(x: nextPoint.position.getX(), y: nextPoint.position.getY()))
            for n in 1...SEGMENTS_PER_PATH {
                let pointNum = Double(n) / Double(SEGMENTS_PER_PATH)
                nextPath.interpolate(pointNum, nextPoint)
                drawingPath.addLine(to: CGPoint(x: nextPoint.position.getX(), y: nextPoint.position.getY()))
            }
        }
        
        drawingPath.apply(transform)
        drawingPath.stroke()
    }
    
}

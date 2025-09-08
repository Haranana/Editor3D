#ifndef TEXTURE_H
#define TEXTURE_H

#include <QImage>
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <Math/Vector3.h>
#include <Math/Vector2.h>
#include <QHash>
#include "Math/Utility.h"
class Texture{
public:
    QImage image;
    QString path;

    //converts each pixel in texture from sRGB to linear
    void toLinear(){
        unsigned char *ptr = image.bits();
        int bytesPerLine = image.bytesPerLine();

        for (int y = 0; y < image.height(); y++) {
            for (int x = 0; x < image.width(); x++) {
                int idx = bytesPerLine * y + x * 4;
                ptr[idx] = 255.0*RendUt::sRGBToLinear(double(ptr[idx])/255.0);
                ptr[idx+1] = 255.0*RendUt::sRGBToLinear(double(ptr[idx+1])/255.0);
                ptr[idx+2] = 255.0*RendUt::sRGBToLinear(double(ptr[idx+2])/255.0);
            }
        }

        linearFlag = true;
    }

    bool isLinear() const {
        return linearFlag;
    }

    void setLinearFlag(bool v){
        linearFlag = v;
    }

    bool linearFlag = false;

    static Vector3 sampleRGB(const std::shared_ptr<Texture>& tex, const Vector2& uv){

        const Vector3 defaultResult(1,1,1);

        if(!tex) return defaultResult;
        QImage& img = tex->image;

        if(img.isNull() || img.width() <= 0 || img.height() <= 0){
            return defaultResult;
        }

        double u = std::clamp(uv.x , 0.0, 1.0);
        double v = std::clamp(uv.y , 0.0, 1.0);

        //flipV should be at import so its not needed here
        int x = int(u * (img.width()  - 1) + 0.5);
        int y = int(v * (img.height() - 1) + 0.5);

        if (x >= 0 && x < img.width() && y >= 0 && y < img.height()) {
            unsigned char *ptr = img.bits();
            int bytesPerLine = img.bytesPerLine();
            int idx = bytesPerLine * y + x * 4;

            int B = ptr[idx];
            int G = ptr[idx + 1];
            int R = ptr[idx + 2];

            return {B/255.0, G/255.0, R/255.0};
        }

        return defaultResult;
    }


    //not currently used
    static inline double sampleA(const std::shared_ptr<Texture>& tex, const Vector2& uv)
    {
        if (!tex) return 1.0;
        const QImage& img = tex->image;
        if (img.isNull() || img.width() <= 0 || img.height() <= 0) return 1.0;

        const double u = std::clamp(uv.x, 0.0, 1.0);
        const double v = std::clamp(uv.y, 0.0, 1.0);
        const int x = int(u * (img.width()  - 1) + 0.5);
        const int y = int(v * (img.height() - 1) + 0.5);

        const QRgb px = img.pixel(x, y);
        const int A = qAlpha(px);
        if (A < 255) return A / 255.0;

        return qGray(px) / 255.0;
    }

    static std::shared_ptr<Texture> loadTextureCached(const QString& path){
        static QHash<QString, std::weak_ptr<Texture>> textures;

        //get path (canonical, if not possible then absolute)
        QFileInfo info(path);
        QString canonical = info.canonicalFilePath();
        if (canonical.isEmpty()){
            canonical = info.absoluteFilePath();
        }

        //if texture already in map, just return it
        if(auto it = textures.find(canonical); it!=textures.end() ){
            if(auto txt = it.value().lock()){
                return txt;
            }
        }

        //load texture from file
        QImage img(!canonical.isEmpty() ? canonical : path);
        if(img.isNull()){
            return nullptr;
        }

        auto txt = std::make_shared<Texture>();
        txt->image = img.convertToFormat(QImage::Format_ARGB32);
        txt->path = canonical;;
        textures[canonical] = txt;

        return txt;
    }

    static QString resolveMapPath(const QString& sourcePath,
                                  const std::string& mapPathFromMtl){
        if (mapPathFromMtl.empty())
            return {};


        QString rawPath = QString::fromStdString(mapPathFromMtl);

        if (rawPath.startsWith('"') && rawPath.endsWith('"') && rawPath.size() >= 2){
            rawPath = rawPath.mid(1, rawPath.size()-2);
        }

        QFileInfo fileInfo(rawPath);
        if (fileInfo.isAbsolute())
            return fileInfo.absoluteFilePath();

        QDir baseDir = QFileInfo(sourcePath).dir();
        QString combined = baseDir.filePath(rawPath);

        return QDir::cleanPath(combined);
    }
};

#endif // TEXTURE_H

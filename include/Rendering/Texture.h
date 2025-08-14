#ifndef TEXTURE_H
#define TEXTURE_H

#include <QImage>
#include <QString>
#include <QFileInfo>
#include <QDir>

class Texture{
public:
    QImage image;
    QString path;

    static std::shared_ptr<Texture> loadTextureCached(const QString& path){
        static std::unordered_map<QString, std::weak_ptr<Texture>> textures;

        //get path (canonical, if not possible then absolute)
        QFileInfo info(path);
        QString canonical = info.canonicalFilePath();
        if (canonical.isEmpty()){
            canonical = info.absoluteFilePath();
        }

        //if texture already in map, just return it
        if(auto it = textures.find(canonical); it!=textures.end() ){
            if(auto txt = it->second.lock()){
                return txt;
            }
        }

        //load texture from file
        QImage img(path);
        if(img.isNull()){
            return nullptr;
        }

        auto txt = std::make_shared<Texture>();
        txt->image = img.convertToFormat(QImage::Format_RGBA8888);
        txt->path = canonical;;
        textures[canonical] = txt;

        return txt;
    }

    static QString resolveMapPath(const QString& sourcePath,
                                  const std::string& mapPathFromMtl){
        if (mapPathFromMtl.empty())
            return {};


        QString rawPath = QString::fromStdString(mapPathFromMtl);

        QFileInfo fileInfo(rawPath);
        if (fileInfo.isAbsolute())
            return fileInfo.absoluteFilePath();

        QDir baseDir = QFileInfo(sourcePath).dir();
        QString combined = baseDir.filePath(rawPath);

        return QDir::cleanPath(combined);
    }
};

#endif // TEXTURE_H

/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "ui/image/image_location_factory.h"

#include "ui/image/image.h"
#include "main/main_session.h"

#include <QtCore/QBuffer>

namespace Images {

ImageWithLocation FromPhotoSize(
		not_null<Main::Session*> session,
		const MTPDphoto &photo,
		const MTPPhotoSize &size) {
	if (!photo.vaccess_hash().v && photo.vfile_reference().v.isEmpty()) {
		// Locally created fake photo.
		return ImageWithLocation();
	}
	return size.match([&](const MTPDphotoSize &data) {
		return ImageWithLocation{
			.location = ImageLocation(
				DownloadLocation{ StorageFileLocation(
					photo.vdc_id().v,
					session->userId(),
					MTP_inputPhotoFileLocation(
						photo.vid(),
						photo.vaccess_hash(),
						photo.vfile_reference(),
						data.vtype())) },
				data.vw().v,
				data.vh().v),
			.bytesCount = data.vsize().v
		};
	}, [&](const MTPDphotoCachedSize &data) {
		const auto bytes = qba(data.vbytes());
		return ImageWithLocation{
			.location = ImageLocation(
				DownloadLocation{ StorageFileLocation(
					photo.vdc_id().v,
					session->userId(),
					MTP_inputPhotoFileLocation(
						photo.vid(),
						photo.vaccess_hash(),
						photo.vfile_reference(),
						data.vtype())) },
				data.vw().v,
				data.vh().v),
			.bytes = bytes,
			.bytesCount = bytes.size(),
		};
	}, [&](const MTPDphotoSizeProgressive &data) {
		// #TODO layer118
		if (data.vsizes().v.isEmpty()) {
			return ImageWithLocation();
		}
		return ImageWithLocation{
			.location = ImageLocation(
				DownloadLocation{ StorageFileLocation(
					photo.vdc_id().v,
					session->userId(),
					MTP_inputPhotoFileLocation(
						photo.vid(),
						photo.vaccess_hash(),
						photo.vfile_reference(),
						data.vtype())) },
				data.vw().v,
				data.vh().v),
			.bytesCount = data.vsizes().v.back().v
		};
	}, [&](const MTPDphotoStrippedSize &data) {
		return ImageWithLocation();
		//const auto bytes = ExpandInlineBytes(qba(data.vbytes()));
		//return ImageWithLocation{
		//	.location = ImageLocation(
		//		DownloadLocation{ StorageFileLocation(
		//			photo.vdc_id().v,
		//			session->userId(),
		//			MTP_inputPhotoFileLocation(
		//				photo.vid(),
		//				photo.vaccess_hash(),
		//				photo.vfile_reference(),
		//				data.vtype())) },
		//		width, // ???
		//		height), // ???
		//	.bytes = bytes,
		//	.bytesCount = bytes.size(),
		//};
	}, [&](const MTPDphotoSizeEmpty &) {
		return ImageWithLocation();
	});
}

ImageWithLocation FromPhotoSize(
		not_null<Main::Session*> session,
		const MTPDdocument &document,
		const MTPPhotoSize &size) {
	return size.match([&](const MTPDphotoSize &data) {
		return ImageWithLocation{
			.location = ImageLocation(
				DownloadLocation{ StorageFileLocation(
					document.vdc_id().v,
					session->userId(),
					MTP_inputDocumentFileLocation(
						document.vid(),
						document.vaccess_hash(),
						document.vfile_reference(),
						data.vtype())) },
				data.vw().v,
				data.vh().v),
			.bytesCount = data.vsize().v
		};
	}, [&](const MTPDphotoCachedSize &data) {
		const auto bytes = qba(data.vbytes());
		return ImageWithLocation{
			.location = ImageLocation(
				DownloadLocation{ StorageFileLocation(
					document.vdc_id().v,
					session->userId(),
					MTP_inputDocumentFileLocation(
						document.vid(),
						document.vaccess_hash(),
						document.vfile_reference(),
						data.vtype())) },
				data.vw().v,
				data.vh().v),
			.bytes = bytes,
			.bytesCount = bytes.size(),
		};
	}, [&](const MTPDphotoSizeProgressive &data) {
		// #TODO layer118
		if (data.vsizes().v.isEmpty()) {
			return ImageWithLocation();
		}
		return ImageWithLocation{
			.location = ImageLocation(
				DownloadLocation{ StorageFileLocation(
					document.vdc_id().v,
					session->userId(),
					MTP_inputDocumentFileLocation(
						document.vid(),
						document.vaccess_hash(),
						document.vfile_reference(),
						data.vtype())) },
				data.vw().v,
				data.vh().v),
			.bytesCount = data.vsizes().v.back().v
		};
	}, [&](const MTPDphotoStrippedSize &data) {
		return ImageWithLocation();
		//const auto bytes = ExpandInlineBytes(qba(data.vbytes()));
		//return ImageWithLocation{
		//	.location = ImageLocation(
		//		DownloadLocation{ StorageFileLocation(
		//			document.vdc_id().v,
		//			session->userId(),
		//			MTP_inputDocumentFileLocation(
		//				document.vid(),
		//				document.vaccess_hash(),
		//				document.vfile_reference(),
		//				data.vtype())) },
		//		width, // ???
		//		height), // ???
		//	.bytes = bytes,
		//	.bytesCount = bytes.size(),
		//};
	}, [&](const MTPDphotoSizeEmpty &) {
		return ImageWithLocation();
	});
}

ImageWithLocation FromPhotoSize(
		not_null<Main::Session*> session,
		const MTPDstickerSet &set,
		const MTPPhotoSize &size) {
	if (!set.vthumb_dc_id()) {
		return ImageWithLocation();
	}
	return size.match([&](const MTPDphotoSize &data) {
		const auto &location = data.vlocation().c_fileLocationToBeDeprecated();
		return ImageWithLocation{
			.location = ImageLocation(
				DownloadLocation{ StorageFileLocation(
					set.vthumb_dc_id()->v,
					session->userId(),
					MTP_inputStickerSetThumb(
						MTP_inputStickerSetID(set.vid(), set.vaccess_hash()),
						location.vvolume_id(),
						location.vlocal_id())) },
				data.vw().v,
				data.vh().v),
			.bytesCount = data.vsize().v
		};
	}, [&](const MTPDphotoCachedSize &data) {
		const auto &location = data.vlocation().c_fileLocationToBeDeprecated();
		const auto bytes = qba(data.vbytes());
		return ImageWithLocation{
			.location = ImageLocation(
				DownloadLocation{ StorageFileLocation(
					set.vthumb_dc_id()->v,
					session->userId(),
					MTP_inputStickerSetThumb(
						MTP_inputStickerSetID(set.vid(), set.vaccess_hash()),
						location.vvolume_id(),
						location.vlocal_id())) },
				data.vw().v,
				data.vh().v),
			.bytes = bytes,
			.bytesCount = bytes.size(),
		};
	}, [&](const MTPDphotoSizeProgressive &data) {
		// #TODO layer118
		if (data.vsizes().v.isEmpty()) {
			return ImageWithLocation();
		}
		const auto &location = data.vlocation().c_fileLocationToBeDeprecated();
		return ImageWithLocation{
			.location = ImageLocation(
				DownloadLocation{ StorageFileLocation(
					set.vthumb_dc_id()->v,
					session->userId(),
					MTP_inputStickerSetThumb(
						MTP_inputStickerSetID(set.vid(), set.vaccess_hash()),
						location.vvolume_id(),
						location.vlocal_id())) },
				data.vw().v,
				data.vh().v),
			.bytesCount = data.vsizes().v.back().v
		};
	}, [&](const MTPDphotoStrippedSize &data) {
		return ImageWithLocation();
		//const auto bytes = ExpandInlineBytes(qba(data.vbytes()));
		//return ImageWithLocation{
		//	.location = ImageLocation(
		//		DownloadLocation{ StorageFileLocation(
		//			document.vdc_id().v,
		//			session->userId(),
		//			MTP_inputDocumentFileLocation(
		//				document.vid(),
		//				document.vaccess_hash(),
		//				document.vfile_reference(),
		//				data.vtype())) },
		//		width, // ???
		//		height), // ???
		//	.bytes = bytes,
		//	.bytesCount = bytes.size(),
		//};
	}, [&](const MTPDphotoSizeEmpty &) {
		return ImageWithLocation();
	});
}

ImageWithLocation FromImageInMemory(
		const QImage &image,
		const char *format) {
	if (image.isNull()) {
		return ImageWithLocation();
	}
	auto bytes = QByteArray();
	auto buffer = QBuffer(&bytes);
	image.save(&buffer, format);
	return ImageWithLocation{
		.location = ImageLocation(
			DownloadLocation{ InMemoryLocation{ bytes } },
			image.width(),
			image.height()),
		.bytes = bytes,
		.preloaded = image,
		.bytesCount = bytes.size(),
	};
}

ImageLocation FromWebDocument(const MTPWebDocument &document) {
	return document.match([](const MTPDwebDocument &data) {
		const auto size = GetSizeForDocument(data.vattributes().v);

		// We don't use size from WebDocument, because it is not reliable.
		// It can be > 0 and different from the real size
		// that we get in upload.WebFile result.
		//auto filesize = 0; // data.vsize().v;
		return ImageLocation(
			DownloadLocation{ WebFileLocation(
				data.vurl().v,
				data.vaccess_hash().v) },
			size.width(),
			size.height());
	}, [](const MTPDwebDocumentNoProxy &data) {
		const auto size = GetSizeForDocument(data.vattributes().v);

		// We don't use size from WebDocument, because it is not reliable.
		// It can be > 0 and different from the real size
		// that we get in upload.WebFile result.
		//auto filesize = 0; // data.vsize().v;
		return ImageLocation(
			DownloadLocation{ PlainUrlLocation{ qs(data.vurl()) } },
			size.width(),
			size.height());
	});
}

ImageWithLocation FromVideoSize(
		not_null<Main::Session*> session,
		const MTPDdocument &document,
		const MTPVideoSize &size) {
	return size.match([&](const MTPDvideoSize &data) {
		return ImageWithLocation{
			.location = ImageLocation(
				DownloadLocation{ StorageFileLocation(
					document.vdc_id().v,
					session->userId(),
					MTP_inputDocumentFileLocation(
						document.vid(),
						document.vaccess_hash(),
						document.vfile_reference(),
						data.vtype())) },
				data.vw().v,
				data.vh().v),
			.bytesCount = data.vsize().v,
		};
	});
}

ImageWithLocation FromVideoSize(
		not_null<Main::Session*> session,
		const MTPDphoto &photo,
		const MTPVideoSize &size) {
	return size.match([&](const MTPDvideoSize &data) {
		return ImageWithLocation{
			.location = ImageLocation(
				DownloadLocation{ StorageFileLocation(
					photo.vdc_id().v,
					session->userId(),
					MTP_inputPhotoFileLocation(
						photo.vid(),
						photo.vaccess_hash(),
						photo.vfile_reference(),
						data.vtype())) },
				data.vw().v,
				data.vh().v),
			.bytesCount = data.vsize().v,
		};
	});
}

} // namespace Images

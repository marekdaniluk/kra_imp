/**
 * kra_imp - kra file import library
 * --------------------------------------------------------
 * Copyright (C) 2024, by Marek Daniluk (@GypsyMagic)
 * This library is distributed under the MIT License.
 */
#pragma once
#include <kra_imp/config.hpp>

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief Defines the maximum length for strings in the kra_imp.
 *
 * @details
 * This macro specifies the maximum number of characters (including the null terminator)
 * that can be stored in string fields used throughout the kra_imp.
 */
#define KRA_IMP_MAX_STRING_LENGTH 255
    /**
     * @ingroup kra_imp
     *
     * @brief Defines error codes used throughout the kra_imp API.
     *
     * @details
     * This enumeration provides standard return codes for functions in the kra_imp API,
     * indicating the success or failure of an operation.
     *
     * @note These error codes are commonly used as return values in API functions
     * for straightforward status checking.
     */
    typedef enum kra_imp_error_code_e
    {
        KRA_IMP_FAIL = 0,                   /**< Indicates the generic operation failed. */
        KRA_IMP_SUCCESS = 1,                /**< Indicates that the operation completed successfully. */
        KRA_IMP_PARAMS_ERROR = -1,          /**< Indicates operation failed due the invalid param(s). */
        KRA_IMP_PARSE_ERROR = -2,           /**< Indicates that the parse operation failed. */
        KRA_IMP_DECOMPRESS_ERROR = -3,      /**< Indicates that the decompress operation failed. */
    } kra_imp_error_code_e;
    /**
     * @ingroup kra_imp
     *
     * @brief Enumerates color space models in the KRA importer.
     *
     * @details
     * This enumeration defines the various color space models that may be encountered in a KRA document.
     * Each value represents a different color space model, which determines how color information is
     * stored and processed for layers or images.
     *
     * @note Currently, only the `KRA_IMP_RGBA` color space model is supported by the KRA importer.
     * Other color spaces are recognized but not processed.
     */
    typedef enum kra_imp_color_space_model_e
    {
        KRA_IMP_UNKNOWN_COLOR_SPACE_MODEL = 0, /**< An unknown or unsupported color space model. */
        KRA_IMP_CIELAB_COLOR_SPACE_MODEL,      /**< The CIELAB color space, commonly used for perceptual uniformity. */
        KRA_IMP_CMYK_COLOR_SPACE_MODEL,        /**< The CMYK color space, primarily used in printing. */
        KRA_IMP_GRAYA_COLOR_SPACE_MODEL,       /**< Grayscale color space with an alpha (transparency) channel. */
        KRA_IMP_RGBA_COLOR_SPACE_MODEL,        /**< Red, Green, Blue, and Alpha (transparency) color space. */
        KRA_IMP_XYZA_COLOR_SPACE_MODEL,        /**< The CIEXYZ color space with an alpha channel. */
        KRA_IMP_YCBCR_COLOR_SPACE_MODEL,       /**< The YCbCr color space, often used in video and image compression. */
    } kra_imp_color_space_model_e;
    /**
     * @ingroup kra_imp
     *
     * @brief Enumerates the types of layers available in a KRA archive.
     *
     * @details
     * This enumeration defines the various types of layers that can exist in a KRA archive.
     * Each layer type represents a specific function or behavior, and these types are
     * used to identify and handle layers appropriately during the import process.
     *
     * @note The layer type is determined when reading layer data, and it helps in
     * reconstructing the layer's behavior and structure during processing.
     */
    typedef enum kra_imp_layer_type_e
    {
        KRA_IMP_UNKNOWN_LAYER_TYPE = 0,      /**< The layer type is unknown or not recognized. */
        KRA_IMP_GROUP_LAYER_TYPE,            /**< A group layer that can contain other layers, forming a hierarchical structure. */
        KRA_IMP_PAINT_LAYER_TYPE,            /**< A standard paint layer used for raster graphics. */
        KRA_IMP_CLONE_LAYER_TYPE,            /**< A clone layer that mirrors content from another layer. Unsupported. */
        KRA_IMP_FILE_LAYER_TYPE,             /**< A file layer that links to an external file. Unsupported. */
        KRA_IMP_COLORIZEMASK_LAYER_TYPE,     /**< A colorize mask layer used for coloring line art. Unsupported. */
        KRA_IMP_TRANSFORMMASK_LAYER_TYPE,    /**< A transform mask layer used for geometric transformations such as scaling or rotation. Unsupported. */
        KRA_IMP_TRANSPARENCYMASK_LAYER_TYPE, /**< A transparency mask layer that modifies the opacity of its parent layer. Unsupported. */
    } kra_imp_layer_type_e;
    /**
     * @ingroup kra_imp
     *
     * @brief Enumerates the visibility states of a layer in the KRA importer.
     *
     * @details
     * This enumeration represents whether a layer is visible or hidden in the imported
     * KRA document.
     */
    typedef enum kra_imp_layer_visibility_e
    {
        KRA_IMP_HIDDEN = 0, /**< The layer is hidden and not displayed in the composition. */
        KRA_IMP_VISIBLE,    /**< The layer is visible and contributes to the composition. */
    } kra_imp_layer_visibility_e;
    /**
     * @struct kra_imp_archive_t
     *
     * @brief Represents an opened KRA archive for reading and processing.
     *
     * @details
     * This structure encapsulates the data of an opened KRA archive,
     * providing the context required for various operations, such as accessing
     * files, layers, key frames, and image properties.
     *
     * The `kra_imp_archive_t` is managed internally by the API and must be created
     * using `kra_imp_open_archive`. Once created, it serves as the primary handle
     * for interacting with the archive's content. It should be properly closed using
     * `kra_imp_close_archive` to release allocated resources.
     *
     * @note The structure's internal implementation is opaque to the user and is
     * fully managed by the API.
     */
    struct KRA_IMP_API kra_imp_archive_t;
    typedef struct kra_imp_archive_t kra_imp_archive_t;
    /**
     * @struct kra_imp_animation_t
     *
     * @brief Represents animation properties in a KRA file.
     *
     * @details
     * This structure contains metadata related to the animation, including the frame rate and the range of frames defined for the animation.
     */
    struct KRA_IMP_API kra_imp_animation_t
    {
        unsigned int _frame_rate; /**< The frame rate of the animation in frames per second. */
        unsigned int _from;       /**< The starting frame of the animation range. */
        unsigned int _to;         /**< The ending frame of the animation range. */
    };
    typedef struct kra_imp_animation_t kra_imp_animation_t;
    /**
     * @struct kra_imp_main_doc_t
     *
     * @brief Represents the main document metadata of a KRA image.
     *
     * @details
     * This structure contains the primary metadata extracted from the main document of a KRA image.
     */
    struct KRA_IMP_API kra_imp_main_doc_t
    {
        char _image_name[KRA_IMP_MAX_STRING_LENGTH];    /**< The name of the image as specified in the main document. */
        kra_imp_color_space_model_e _color_space_model; /**< The color space model used by the image (e.g., RGBA, CMYK). */
        unsigned int _layers_count;                     /**< The total number of layers in the image. */
        unsigned int _height;                           /**< The height of the image in pixels. */
        unsigned int _width;                            /**< The width of the image in pixels. */
        kra_imp_animation_t _animation;                 /**< The animation properties associated with the image. */
    };
    typedef struct kra_imp_main_doc_t kra_imp_main_doc_t;
    /**
     * @struct kra_imp_image_layer_t
     *
     * @brief Represents a single layer in a KRA image.
     *
     * @details
     * This structure stores information about an individual layer in a KRA image. Layers are part of a
     * tree structure that can be reconstructed using the parent index information.
     */
    struct KRA_IMP_API kra_imp_image_layer_t
    {
        char _name[KRA_IMP_MAX_STRING_LENGTH];            /**< The name of the layer. */
        char _file_name[KRA_IMP_MAX_STRING_LENGTH];       /**< The file name associated with the layer's content. */
        char _frame_file_name[KRA_IMP_MAX_STRING_LENGTH]; /**< The file name of the keyframe associated with the layer, if applicable. */
        unsigned char _opacity;                           /**< The opacity of the layer, ranging from 0 (completely transparent) to 255 (fully opaque). */
        kra_imp_layer_visibility_e _visibility;           /**< The visibility state of the layer, as defined by `kra_imp_layer_visibility_e`. */
        kra_imp_layer_type_e _type;                       /**< The type of the layer, defined by `kra_imp_layer_type_e`. */
        long _parent_index;                               /**< The index of the parent layer, or `-1` if the layer has no parent. */
    };
    typedef struct kra_imp_image_layer_t kra_imp_image_layer_t;
    /**
     * @struct kra_imp_image_key_frame_t
     *
     * @brief Represents an individual keyframe in a KRA image animation.
     *
     * @details
     * This structure stores information about a single keyframe in an animated image within a KRA archive.
     */
    struct KRA_IMP_API kra_imp_image_key_frame_t
    {
        char _frame[KRA_IMP_MAX_STRING_LENGTH]; /**< Name of the keyframe. */
        int _x;                                 /**< Horizontal position of the keyframe in the image's coordinate space. */
        int _y;                                 /**< Vertical position of the keyframe in the image's coordinate space. */
        unsigned int _time;                     /**< Time of the keyframe in the animation timeline, in milliseconds. */
        unsigned int _key_frames_count;         /**< Total number of keyframes in the animation. */
    };
    typedef struct kra_imp_image_key_frame_t kra_imp_image_key_frame_t;
    /**
     * @struct kra_imp_layer_data_header_t
     *
     * @brief Represents data about a layer's tiles in a KRA archive.
     *
     * @details
     * This structure holds essential information about a layer's data tiles,
     * including its size, dimensions, and versioning. It is used to manage and
     * interpret tile-based data efficiently when reading layer data from a KRA archive.
     *
     * @note The internal fields should not be modified by the user directly.
     * They are managed by the API to ensure correct data interpretation.
     */
    struct KRA_IMP_API kra_imp_layer_data_header_t
    {
        unsigned int _header_size;           /**< The size of the header within the buffer. */
        unsigned int _layer_datas_count;     /**< The number of tiles in the layer. */
        unsigned int _layer_data_pixel_size; /**< The size of pixels in the data tiles. */
        unsigned int _layer_data_width;      /**< Width dimension of each data tile in pixels. */
        unsigned int _layer_data_height;     /**< Height dimension of each data tile in pixels. */
        unsigned int _version;               /**< Version of the layer data format. */
    };
    typedef struct kra_imp_layer_data_header_t kra_imp_layer_data_header_t;
#ifdef __cplusplus
}
#endif
